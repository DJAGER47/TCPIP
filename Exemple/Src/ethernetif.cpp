#include "stm32f7xx_hal.h"
#include "ethernetif.h"
#include "lan8742.h"
#include <string.h>

#include "debug.h"
#include "stupidAllocator.h"



static ETH_DMADescTypeDef gDMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
static ETH_DMADescTypeDef gDMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
static stupidAllocator<ETH_RX_DESC_CNT> sA;
static osSemaphoreId gRxSemaphore = NULL; /* Semaphore to signal incoming packets */
static osSemaphoreId gTxSemaphore = NULL; /* Semaphore to signal transmit packet complete */

/* Global Ethernet handle */
ETH_HandleTypeDef gEthHandle;
static ETH_TxPacketConfig gTxConfig;
static lan8742_Object_t gLAN8742;
extern lan8742_IOCtx_t  LAN8742_IOCtx;


/* Private function prototypes -----------------------------------------------*/
void readEth( void const * argument );
void ethernet_link_thread( void const * argument );
void writeEth( void const * argument );






                               

#define MAC_ADDR 0x02,0x00,0x00,0x00,0x00,0x00
void low_level_init()
{
  uint32_t duplex, speed = 0;
  int32_t PHYLinkState = 0;
  uint8_t macaddress[6]= {MAC_ADDR};
  gEthHandle.Instance = ETH;
  gEthHandle.Init.MACAddr = macaddress;
  gEthHandle.Init.MediaInterface = HAL_ETH_RMII_MODE;
  gEthHandle.Init.RxDesc = gDMARxDscrTab;
  gEthHandle.Init.TxDesc = gDMATxDscrTab;
  gEthHandle.Init.RxBuffLen = ETH_RX_BUF_SIZE;

  /* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
  HAL_ETH_Init(&gEthHandle);

  /* Set Tx packet config common parameters */
  memset(&gTxConfig, 0 , sizeof(ETH_TxPacketConfig));
  gTxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  gTxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  gTxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

  gRxSemaphore = xSemaphoreCreateBinary();
  gTxSemaphore = xSemaphoreCreateBinary();

  /* create the task that handles the ETH_MAC */
  osThreadDef(EthIf, readEth, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(EthIf), NULL);


  // osThreadDef(Link, ethernet_link_thread, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
  // osThreadCreate (osThread(Link), NULL);

  osThreadDef(Send, writeEth, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(Send), NULL);


  /* Set PHY IO functions */
  LAN8742_RegisterBusIO(&gLAN8742, &LAN8742_IOCtx);

  /* Initialize the LAN8742 ETH PHY */
  LAN8742_Init(&gLAN8742);

  PHYLinkState = LAN8742_GetLinkState(&gLAN8742);
  _printf("PHYLinkState %d\n", PHYLinkState);

  /* Get link state */
  if(PHYLinkState <= LAN8742_STATUS_LINK_DOWN)
  {
    _printf("LAN8742_STATUS_LINK_DOWN\n");
  }
  else
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
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    }

    /* Get MAC Config MAC */
    ETH_MACConfigTypeDef MACConf;
    HAL_ETH_GetMACConfig(&gEthHandle, &MACConf);
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
    HAL_ETH_SetMACConfig(&gEthHandle, &MACConf);
    HAL_ETH_Start_IT(&gEthHandle);
  }
}


void writeEth( void const * argument )
{
  UNUSED(argument);
  ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
//  uint8_t data[] = {
//    0xff,0xff,0xff,0xff,0xff,0xff,0x30,0x24,0xa9,0x87,0x3e,0x01,0x08,0x06,0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x30,
//    0x24,0xa9,0x87,0x3e,0x01,0xc0,0xa8,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,0x00,0x14
//  };
  
  #define  IP_UnicastAddress    192, 168, 0, 20
  #define MAC_UnicastAddress    0x00, 0x80, 0xE1, 0x00, 0x00, 0x00

  #define  IP_PC_UnicastAddress 192, 168, 0, 40
  #define MAC_PC_UnicastAddress 0x30, 0x24, 0xA9, 0x87, 0x3E, 0x01

  uint8_t arp_pkg_replay[] = {
    MAC_UnicastAddress,
    MAC_PC_UnicastAddress,
    0x08, 0x06,

    0x00, 0x01,
    0x08, 0x00,
    0x06,
    0x04,
    0x00, 0x02,
    MAC_UnicastAddress,
    IP_UnicastAddress,
    MAC_PC_UnicastAddress,
    IP_PC_UnicastAddress,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));

  Txbuffer[0].buffer = arp_pkg_replay;
  Txbuffer[0].len = sizeof arp_pkg_replay;
  Txbuffer[0].next = NULL;

  gTxConfig.Length = sizeof arp_pkg_replay;
  gTxConfig.TxBuffer = Txbuffer;
  gTxConfig.pData = arp_pkg_replay;

  while(1){
    osDelay(5000);

    HAL_ETH_Transmit_IT(&gEthHandle, &gTxConfig);
    _printf("Transmit_IT\n");
    while (osSemaphoreWait(gTxSemaphore, osWaitForever) != osOK){}
    _printf("ReleaseTxPacket\n");
    HAL_ETH_ReleaseTxPacket(&gEthHandle);
  }
}


/// @brief  Check the ETH link state and update netif accordingly.
void ethernet_link_thread( void const * argument )
{
  UNUSED(argument);
  int32_t PHYLinkState = 0;
  uint32_t speed = 0, duplex =0;

  for(;;)
  {
    int32_t state = LAN8742_GetLinkState(&gLAN8742);
    if (PHYLinkState != state)
    {
      PHYLinkState = state;
      _printf("PHYLinkState %d\n", PHYLinkState);

      if(PHYLinkState <= LAN8742_STATUS_LINK_DOWN)
      {
        _printf("Stop_IT\n");
        HAL_ETH_Stop_IT(&gEthHandle);
      }
      else //if(PHYLinkState > LAN8742_STATUS_LINK_DOWN)
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
          _printf("default\n");
          break;
        }

        /* Get MAC Config MAC */
        ETH_MACConfigTypeDef MACConf;
        HAL_ETH_GetMACConfig(&gEthHandle, &MACConf);
        MACConf.DuplexMode = duplex;
        MACConf.Speed = speed;
        HAL_ETH_SetMACConfig(&gEthHandle, &MACConf);

        _printf("Start_IT\n");
        HAL_ETH_Start_IT(&gEthHandle);
      }
    }

    osDelay(100);
  }
}


void readEth( void const * argument )
{
  UNUSED(argument);
  RxBuff_t *p = NULL;

  for( ;; )
  {
    if (osSemaphoreWait( gRxSemaphore, osWaitForever)==osOK)
    {
      _printf("ReadData\n");

      do
      {
        HAL_ETH_ReadData(&gEthHandle, (void **)&p);

        if (p != NULL)
        {
          // read
          _printf("Input len %d\n", p->pbuf.len);
          if (p->pbuf.len < 100)
            for (uint32_t i = 0; i < p->pbuf.len; ++i)
            {
              _printf("%02x ", ((uint8_t*)p->pbuf.payload)[i]);
            }
          _printf("\n");

          auto tmp = p->pbuf.next;
          sA.release(p);
          p = (RxBuff_t *)tmp;
        }
      }while(p != NULL);
    }
  }
}


/// @brief  Ethernet Rx Transfer completed callback
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  UNUSED(heth);
  _printf("IRQ RxCpltCallback\n");
  osSemaphoreRelease(gRxSemaphore);
}

/// @brief  Ethernet Tx Transfer completed callback
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  UNUSED(heth);
  _printf("IRQ TxCpltCallback\n");
  osSemaphoreRelease(gTxSemaphore);
}

/// @brief  Ethernet DMA transfer error callback
void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
  UNUSED(heth);
  _printf("IRQ ErrorCallback\n");
  if((HAL_ETH_GetDMAError(heth) & ETH_DMASR_RBUS) == ETH_DMASR_RBUS)
  {
     osSemaphoreRelease(gRxSemaphore);
  }
}

void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
  _printf("IRQ RxAllocateCallback\n");
  auto p = sA.allocate();
  p->pbuf.payload = p->buff;
  *buff = (uint8_t *)p + offsetof(RxBuff_t, buff);
}

void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length)
{
  _printf("IRQ RxLinkCallback %d\n", Length);

  struct pbuf **ppStart = (struct pbuf **)pStart;
  struct pbuf **ppEnd = (struct pbuf **)pEnd;
  struct pbuf *p = NULL;

  /* Get the struct pbuf from the buff address. */
  p = (struct pbuf *)(buff - offsetof(RxBuff_t, buff));
  p->next = NULL;
  p->tot_len = 0;
  p->len = Length;

  /* Chain the buffer. */
  if (!*ppStart)
  {
    /* The first buffer of the packet. */
    *ppStart = p;
  }
  else
  {
    /* Chain the buffer to the end of the packet. */
    (*ppEnd)->next = p;
  }
  *ppEnd  = p;

  /* Update the total length of all the buffers of the chain. Each pbuf in the chain should have its tot_len
   * set to its own length, plus the length of all the following pbufs in the chain. */
  for (p = *ppStart; p != NULL; p = p->next)
  {
    p->tot_len += Length;
  }
}

void HAL_ETH_TxFreeCallback(uint32_t * buff)
{
  UNUSED(buff);
  _printf("!!!!!!!!!!!!!!! IRQ TxFreeCallback\n");
  //pbuf_free((struct pbuf *)buff);
}



