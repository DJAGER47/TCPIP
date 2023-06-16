#include "ethernetif.h"
#include <string.h>

#include "stupidAllocator.h"
#include "DefaultStack.h"

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
static const TCPIP::IPv4::AddressIP4Settings ADDR_IP = {{192, 168, 0, 20}, {255, 255, 255, 0}, {192, 168, 0, 1}};

static ETH_DMADescTypeDef gDMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((used)); // Ethernet Rx DMA Descriptors
static ETH_DMADescTypeDef gDMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((used)); // Ethernet Tx DMA Descriptors
static TCPIP::stupidAllocator<ETH_RX_DESC_CNT> sA_Rx; // Memory allocator for reception
static osSemaphoreId gRxSemaphore = NULL; // Semaphore to signal incoming packets
static osSemaphoreId gTxSemaphore = NULL; // Semaphore to signal transmit packet complete

// Global Ethernet handle
ETH_HandleTypeDef gEthHandle;
static ETH_TxPacketConfig gTxConfig;
static lan8742_Object_t gLAN8742;
extern lan8742_IOCtx_t LAN8742_IOCtx;
static TCPIP::DefaultStack gDStack;

// Private function prototypes -----------------------------------------------
void readEth(void const *argument);
void ethernet_link_thread(void const *argument);
void writeEth(void const *argument);

void low_level_init()
{
    uint32_t duplex, speed = 0;
    int32_t PHYLinkState = 0;
    gEthHandle.Instance = ETH;
    gEthHandle.Init.MACAddr = const_cast<uint8_t *>(ADDR_MAC);
    gEthHandle.Init.MediaInterface = HAL_ETH_RMII_MODE;
    gEthHandle.Init.RxDesc = gDMARxDscrTab;
    gEthHandle.Init.TxDesc = gDMATxDscrTab;
    gEthHandle.Init.RxBuffLen = ETH_RX_BUF_SIZE;
    HAL_ETH_Init(&gEthHandle);

    /* Set Tx packet config common parameters */
    memset(&gTxConfig, 0, sizeof(ETH_TxPacketConfig));
    gTxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
    gTxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
    gTxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

    osThreadDef(Link, ethernet_link_thread, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
    osThreadCreate (osThread(Link), NULL);

    gRxSemaphore = xSemaphoreCreateBinary();
    gTxSemaphore = xSemaphoreCreateBinary();

    osThreadDef(Read, readEth, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
    osThreadCreate(osThread(Read), NULL);

    // osThreadDef(Write, writeEth, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);
    // osThreadCreate(osThread(Write), NULL);

    gDStack.mac_.SetUnicastAddress(ADDR_MAC);
    gDStack.ip_.SetAddressInfo(ADDR_IP);

    /* Set PHY IO functions */
    LAN8742_RegisterBusIO(&gLAN8742, &LAN8742_IOCtx);
    /* Initialize the LAN8742 ETH PHY */
    LAN8742_Init(&gLAN8742);
    PHYLinkState = LAN8742_GetLinkState(&gLAN8742);
    _printf_mac("PHYLinkState %d\n", PHYLinkState);
    /* Get link state */
    if (PHYLinkState <= LAN8742_STATUS_LINK_DOWN)
    {
        _printf_mac("LAN8742_STATUS_LINK_DOWN\n");
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

void writeEth(void const *argument)
{
    UNUSED(argument);
    ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
    //  uint8_t data[] = {
    //    0xff,0xff,0xff,0xff,0xff,0xff,0x30,0x24,0xa9,0x87,0x3e,0x01,0x08,0x06,0x00,0x01,0x08,0x00,0x06,0x04,0x00,0x01,0x30,
    //    0x24,0xa9,0x87,0x3e,0x01,0xc0,0xa8,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,0x00,0x14
    //  };

#define IP_UnicastAddress 192, 168, 0, 20
#define MAC_UnicastAddress 0x00, 0x80, 0xE1, 0x00, 0x00, 0x00

#define IP_PC_UnicastAddress 192, 168, 0, 40
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
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    memset(Txbuffer, 0, ETH_TX_DESC_CNT * sizeof(ETH_BufferTypeDef));

    Txbuffer[0].buffer = arp_pkg_replay;
    Txbuffer[0].len = sizeof arp_pkg_replay;
    Txbuffer[0].next = NULL;

    gTxConfig.Length = sizeof arp_pkg_replay;
    gTxConfig.TxBuffer = Txbuffer;
    gTxConfig.pData = arp_pkg_replay;

    while (1)
    {
        osDelay(5000);

        HAL_ETH_Transmit_IT(&gEthHandle, &gTxConfig);
        _printf_mac("Transmit_IT\n");
        while (osSemaphoreWait(gTxSemaphore, osWaitForever) != osOK)
        {
        }
        _printf_mac("ReleaseTxPacket\n");
        HAL_ETH_ReleaseTxPacket(&gEthHandle);
    }
}

void readEth(void const *argument)
{
    UNUSED(argument);
    TCPIP::EthBuff *p = NULL;

    while (1)
    {
        if (osSemaphoreWait(gRxSemaphore, osWaitForever) == osOK)
        {
            _printf_mac("ReadData\n");
            while (1)
            {
                if ((HAL_ETH_ReadData(&gEthHandle, (void **)&p) == HAL_OK) && (p != NULL))
                {
                    gDStack.mac_.ProcessRx(p);
                    _printf_mac("Input len %d\n", p->pbuf.len);
                    for (uint32_t i = 0; i < p->pbuf.len; ++i)
                    {
                        _printf_mac("%02x ", ((uint8_t *)p->pbuf.payload)[i]);
                    }
                    _printf_mac("\n");
                    sA_Rx.release(p);
                }
                else
                {
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
    auto p = sA_Rx.allocate();
    p->pbuf.payload = p->buff;
    *buff = (uint8_t *)p + offsetof(TCPIP::EthBuff, buff);
}

void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length)
{
    _printf_mac("IRQ RxLinkCallback %d\n", Length);

    struct TCPIP::pbuf **ppStart = (struct TCPIP::pbuf **)pStart;
    struct TCPIP::pbuf **ppEnd = (struct TCPIP::pbuf **)pEnd;
    struct TCPIP::pbuf *p = NULL;

    /* Get the struct pbuf from the buff address. */
    p = (struct TCPIP::pbuf *)(buff - offsetof(TCPIP::EthBuff, buff));
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
    *ppEnd = p;

    /* Update the total length of all the buffers of the chain. Each pbuf in the chain should have its tot_len
     * set to its own length, plus the length of all the following pbufs in the chain. */
    for (p = *ppStart; p != NULL; p = p->next)
    {
        p->tot_len += Length;
    }
}

void HAL_ETH_TxFreeCallback(uint32_t *buff)
{
    UNUSED(buff);
    _printf_mac("!!!!!!!!!!!!!!! IRQ TxFreeCallback\n");
    // pbuf_free((struct pbuf *)buff);
}

/// @brief  Check the ETH link state and update netif accordingly.
void ethernet_link_thread(void const *argument)
{
    UNUSED(argument);
    int32_t PHYLinkState = 0;
    uint32_t speed = 0, duplex = 0;

    while(1)
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
