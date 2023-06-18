#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f7xx_hal.h"

  void low_level_init(void);

  void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth);
  void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth);
  void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth);
  void HAL_ETH_RxAllocateCallback(uint8_t **buff);
  void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length);
  void HAL_ETH_TxFreeCallback(uint32_t *buff);

#ifdef __cplusplus
}
#endif