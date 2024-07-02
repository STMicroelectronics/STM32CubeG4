/**
  ******************************************************************************
  * @file    BSP/Src/bus.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the bus feature in the
  *          STM32G474 EVAL1 driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Bus demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Bus_demo(void)
{
  int32_t  result  = 0;
  uint32_t i;
  uint8_t  data[2] = {0xFFU, 0xFFU};
  uint8_t  value;

  printf("TEST OF BUS\n");

  /* Initialize I2C3 bus */
  if (BSP_I2C3_Init() != BSP_ERROR_NONE) result--;

  /******************************/
  /* Check if devices are ready */
  /******************************/
  /* IO expander */
  if (BSP_I2C3_IsReady(IO_I2C_ADDRESS, 1) != BSP_ERROR_NONE) result--;
  /* PNA : To complete with other devices */

  /***************************/
  /* Check read/write access */
  /***************************/
  /* IO Expander (only 8bits registers) */
  if (BSP_I2C3_ReadReg(IO_I2C_ADDRESS, MFXSTM32L152_REG_ADR_SYS_CTRL, &value, 1) != BSP_ERROR_NONE) result--;
  /* Enable GPIO expander function */
  value = value | 0x1U;
  if (BSP_I2C3_WriteReg(IO_I2C_ADDRESS, MFXSTM32L152_REG_ADR_SYS_CTRL, &value, 1) != BSP_ERROR_NONE) result--;

  /* Write/read registers GPIO_PUPDx */
  if (BSP_I2C3_WriteReg(IO_I2C_ADDRESS, MFXSTM32L152_REG_ADR_GPIO_PUPD1, data, 2) != BSP_ERROR_NONE) result--;
  HAL_Delay(100);
  if (BSP_I2C3_ReadReg(IO_I2C_ADDRESS, MFXSTM32L152_REG_ADR_GPIO_PUPD1, data, 2) != BSP_ERROR_NONE) result--;
  for (i = 0; i < 2; i++)
  {
    if (data[i] != 0xFFU) result--;
    data[i] = 0U;
  }
  /* Restore default value of  registers GPIO_PUPDx */
  if (BSP_I2C3_WriteReg(IO_I2C_ADDRESS, MFXSTM32L152_REG_ADR_GPIO_PUPD1, data, 2) != BSP_ERROR_NONE) result--;
  HAL_Delay(100);

  /* PNA : To complete with other devices */

  /* De-initialize I2C3 bus */
  if (BSP_I2C3_DeInit() != BSP_ERROR_NONE) result--;

  if (result == 0)  printf("      Passed\n");
      else     printf("      Failed\n");

  return result;
}


/**
  * @}
  */

/**
  * @}
  */


