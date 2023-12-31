#include "ethernetif.h"
#include <string.h>

#include "DefaultStack.h"
#include "InterfaceBuffer.h"
#include "cmsis_os.h"
#include "lan8742.h"
#include "debug.h"

#define DEBUG_MAC 0
#if DEBUG_MAC
#define _printf_mac(...) _printf(__VA_ARGS__)
#else
#define _printf_mac(...)
#endif

static const uint8_t ADDR_MAC[] = {0x00, 0x80, 0xE1, 0x00, 0x00, 0x00};
static const TCPIP::IPv4::AddressIP4Settings ADDR_IP = {{192, 168, 0, 20},
                                                        {255, 255, 255, 0},
                                                        {192, 168, 0, 1}};

static ETH_DMADescTypeDef gDMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((used)); // Ethernet Rx DMA Descriptors
static ETH_DMADescTypeDef gDMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((used)); // Ethernet Tx DMA Descriptors
static osSemaphoreId gRxSemaphore = NULL;                                       // Semaphore to signal incoming packets
static osSemaphoreId gTxSemaphore = NULL;                                       // Semaphore to signal transmit packet complete

// Global Ethernet handle
ETH_HandleTypeDef gEthHandle;
static ETH_TxPacketConfig gTxConfig;
static lan8742_Object_t gLAN8742;
extern lan8742_IOCtx_t LAN8742_IOCtx;
static TCPIP::DefaultStack gDStack;

// Private function prototypes -----------------------------------------------
void readEth(void const *argument);
void ethernet_link_thread(void const *argument);
void writeEth(TCPIP::EthBuff *data);

void low_level_init()
{
  int32_t PHYLinkState = 0;
  gEthHandle.Instance = ETH;
  gEthHandle.Init.MACAddr = const_cast<uint8_t *>(ADDR_MAC);
  gEthHandle.Init.MediaInterface = HAL_ETH_RMII_MODE;
  gEthHandle.Init.RxDesc = gDMARxDscrTab;
  gEthHandle.Init.TxDesc = gDMATxDscrTab;
  gEthHandle.Init.RxBuffLen = ETH_RX_BUF_SIZE;
  HAL_ETH_Init(&gEthHandle);

  // Set Tx packet config common parameters
  memset(&gTxConfig, 0, sizeof(ETH_TxPacketConfig));
  gTxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  gTxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  gTxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

  osThreadDef(Link, ethernet_link_thread, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(Link), NULL);

  gRxSemaphore = xSemaphoreCreateBinary();
  gTxSemaphore = xSemaphoreCreateBinary();
  osSemaphoreRelease(gTxSemaphore);

  osThreadDef(Read, readEth, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate(osThread(Read), NULL);

  gDStack.mac_.SetUnicastAddress(ADDR_MAC);
  gDStack.ip_.SetAddressInfo(ADDR_IP);
  gDStack.mac_.RegisterDataTransmitHandler(writeEth);

  LAN8742_RegisterBusIO(&gLAN8742, &LAN8742_IOCtx); // Set PHY IO functions
  LAN8742_Init(&gLAN8742);                          // Initialize the LAN8742 ETH PHY
  PHYLinkState = LAN8742_GetLinkState(&gLAN8742);
  _printf_mac("PHYLinkState %d\n", PHYLinkState);
  if (PHYLinkState <= LAN8742_STATUS_LINK_DOWN) // Get link state
  {
    _printf_mac("LAN8742_STATUS_LINK_DOWN\n");
  }
  else
  {
    uint32_t duplex, speed;
    switch (PHYLinkState)
    {
    case LAN8742_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    case LAN8742_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    }

    ETH_MACConfigTypeDef MACConf; // Get MAC Config MAC
    HAL_ETH_GetMACConfig(&gEthHandle, &MACConf);
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
    HAL_ETH_SetMACConfig(&gEthHandle, &MACConf);
    HAL_ETH_Start_IT(&gEthHandle);
  }
}

void writeEth(TCPIP::EthBuff *data)
{
  osSemaphoreWait(gTxSemaphore, osWaitForever);
  _printf_mac("WriteData\n");

  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
  memset(Txbuffer, 0, ETH_TX_DESC_CNT * sizeof(ETH_BufferTypeDef));

  size_t i = 0;
  for (TCPIP::EthBuff *q = data; q != NULL; q = q->next)
  {
    if (i >= ETH_TX_DESC_CNT)
    {
      _printf_mac("Exceeding the number of descriptors\n");
      return;
    }

    Txbuffer[i].buffer = q->buff;
    Txbuffer[i].len = q->len;
    if (i > 0)
    {
      Txbuffer[i - 1].next = &Txbuffer[i];
    }
    if (q->next == NULL)
    {
      Txbuffer[i].next = NULL;
    }
    ++i;
  }

  gTxConfig.Length = data->tot_len;
  gTxConfig.TxBuffer = Txbuffer;
  gTxConfig.pData = data;

  HAL_ETH_Transmit_IT(&gEthHandle, &gTxConfig);
}

void readEth(void const *argument)
{
  UNUSED(argument);
  TCPIP::EthBuff *p = nullptr;

  while (1)
  {
    if (osSemaphoreWait(gRxSemaphore, osWaitForever) == osOK)
    {
      _printf_mac("ReadData\n");
      while (1)
      {
        if ((HAL_ETH_ReadData(&gEthHandle, (void **)&p) == HAL_OK) && (p != nullptr))
        {
          gDStack.mac_.ProcessRx(p);
          _printf_mac("Input len %d\n", p->len);
          for (uint32_t i = 0; i < p->len; ++i)
          {
            _printf_mac("%02x ", p->buff[i]);
          }
          _printf_mac("\n");
          gDStack.mac_.FreeRxBuffer(p);
        }
        else
        {
          _printf_mac("r break\n");
          break;
        }
      }
    }
  }
}

/// @brief  Ethernet Rx Transfer completed callback
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  UNUSED(heth);
  _printf_mac("IRQ Rx completed\n");
  osSemaphoreRelease(gRxSemaphore);
}

/// @brief  Ethernet Tx Transfer completed callback
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  UNUSED(heth);
  _printf_mac("IRQ Tx completed\n");
  HAL_ETH_ReleaseTxPacket(&gEthHandle);
  osSemaphoreRelease(gTxSemaphore);
}

/// @brief  Ethernet DMA transfer error callback
void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
  UNUSED(heth);
  _printf_mac("IRQ ErrorCallback\n");
  if ((HAL_ETH_GetDMAError(heth) & ETH_DMASR_RBUS) == ETH_DMASR_RBUS)
  {
    osSemaphoreRelease(gRxSemaphore);
  }
}

void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
  _printf_mac("IRQ RxAllocateCallback\n");
  auto p = gDStack.mac_.GetRxBuffer();
  *buff = p->buff;
}

void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length)
{
  _printf_mac("IRQ RxLinkCallback %d\n", Length);

  TCPIP::EthBuff **ppStart = (TCPIP::EthBuff **)pStart;
  TCPIP::EthBuff **ppEnd = (TCPIP::EthBuff **)pEnd;

  // calculate the address EthBuff
  TCPIP::EthBuff *p = (TCPIP::EthBuff *)(buff - offsetof(TCPIP::EthBuff, buff));
  p->next = nullptr;
  p->tot_len = 0;
  p->len = Length;

  if (!*ppStart)
  {
    *ppStart = p; // The first buffer of the packet.
  }
  else
  {
    (*ppEnd)->next = p; // Chain the buffer to the end of the packet.
  }
  *ppEnd = p;

  // Update the total length of all the buffers of the chain.
  for (p = *ppStart; p != nullptr; p = p->next)
  {
    p->tot_len += Length;
  }
}

void HAL_ETH_TxFreeCallback(uint32_t *buff)
{
  _printf_mac("IRQ TxFreeCallback\n");
  TCPIP::EthBuff *p = reinterpret_cast<TCPIP::EthBuff *>(buff);
  gDStack.mac_.FreeTxBuffer(p);
}

/// @brief  Check the ETH link state and update netif accordingly.
void ethernet_link_thread(void const *argument)
{
  UNUSED(argument);
  int32_t PHYLinkState = 0;
  uint32_t speed = 0, duplex = 0;

  while (1)
  {
    int32_t state = LAN8742_GetLinkState(&gLAN8742);
    if (PHYLinkState != state)
    {
      PHYLinkState = state;
      _printf_mac("PHYLinkState %d\n", PHYLinkState);

      if (PHYLinkState <= LAN8742_STATUS_LINK_DOWN)
      {
        _printf_mac("Stop_IT\n");
        HAL_ETH_Stop_IT(&gEthHandle);
      }
      else // if(PHYLinkState > LAN8742_STATUS_LINK_DOWN)
      {
        switch (PHYLinkState)
        {
        case LAN8742_STATUS_100MBITS_FULLDUPLEX:
          duplex = ETH_FULLDUPLEX_MODE;
          speed = ETH_SPEED_100M;
          break;
        case LAN8742_STATUS_100MBITS_HALFDUPLEX:
          duplex = ETH_HALFDUPLEX_MODE;
          speed = ETH_SPEED_100M;
          break;
        case LAN8742_STATUS_10MBITS_FULLDUPLEX:
          duplex = ETH_FULLDUPLEX_MODE;
          speed = ETH_SPEED_10M;
          break;
        case LAN8742_STATUS_10MBITS_HALFDUPLEX:
          duplex = ETH_HALFDUPLEX_MODE;
          speed = ETH_SPEED_10M;
          break;
        default:
          _printf_mac("default\n");
          break;
        }

        /* Get MAC Config MAC */
        ETH_MACConfigTypeDef MACConf;
        HAL_ETH_GetMACConfig(&gEthHandle, &MACConf);
        MACConf.DuplexMode = duplex;
        MACConf.Speed = speed;
        HAL_ETH_SetMACConfig(&gEthHandle, &MACConf);

        _printf_mac("ReStart_IT\n");
        HAL_ETH_Start_IT(&gEthHandle);
      }
    }
    osDelay(100);
  }
}
