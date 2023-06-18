#include "stm32f7xx_hal.h"
#include "ethernetif.h"
#include "lan8742.h"
#include <string.h>

extern ETH_HandleTypeDef gEthHandle;

int32_t ETH_PHY_IO_Init(void);
int32_t ETH_PHY_IO_DeInit(void);
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
uint32_t ETH_PHY_IO_GetTick(void);

lan8742_IOCtx_t LAN8742_IOCtx = {ETH_PHY_IO_Init,
                                 ETH_PHY_IO_DeInit,
                                 ETH_PHY_IO_WriteReg,
                                 ETH_PHY_IO_ReadReg,
                                 ETH_PHY_IO_GetTick};

/*******************************************************************************
                       PHI IO Functions
*******************************************************************************/
/**
 * @brief  Initializes the MDIO interface GPIO and clocks.
 * @param  None
 * @retval 0 if OK, -1 if ERROR
 */
int32_t ETH_PHY_IO_Init(void)
{
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here
  */

  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&gEthHandle);

  return 0;
}

/**
 * @brief  De-Initializes the MDIO interface .
 * @param  None
 * @retval 0 if OK, -1 if ERROR
 */
int32_t ETH_PHY_IO_DeInit(void)
{
  return 0;
}

/**
 * @brief  Read a PHY register through the MDIO interface.
 * @param  DevAddr: PHY port address
 * @param  RegAddr: PHY register address
 * @param  pRegVal: pointer to hold the register value
 * @retval 0 if OK -1 if Error
 */
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if (HAL_ETH_ReadPHYRegister(&gEthHandle, DevAddr, RegAddr, pRegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

/**
 * @brief  Write a value to a PHY register through the MDIO interface.
 * @param  DevAddr: PHY port address
 * @param  RegAddr: PHY register address
 * @param  RegVal: Value to be written
 * @retval 0 if OK -1 if Error
 */
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if (HAL_ETH_WritePHYRegister(&gEthHandle, DevAddr, RegAddr, RegVal) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

/**
 * @brief  Get the time in millisecons used for internal PHY driver process.
 * @retval Time value
 */
uint32_t ETH_PHY_IO_GetTick(void)
{
  return HAL_GetTick();
}
