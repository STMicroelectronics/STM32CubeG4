/**
  ******************************************************************************
  * @file    utils.c
  * @author  MCD Application Team
  * @brief   STM32G474E_EVAL demonstration firmware utilities
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "utils.h"
#include "stm32g4xx_hal.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "stm32_lcd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Backup register management */
#define BKUP_IMAGE_ID       RTC_BKP_DR0
#define BKUP_BOOT_REASON    RTC_BKP_DR1
#define BKUP_RTC_INIT       RTC_BKP_DR2

/* Private function prototypes -----------------------------------------------*/
/* Private Variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern RTC_HandleTypeDef RtcHandle;

/* Exported functions --------------------------------------------------------*/
void UTILS_SetImageId(uint32_t ImageId)
{
  HAL_RTCEx_BKUPWrite(&RtcHandle, BKUP_IMAGE_ID, ImageId);
}

uint32_t UTILS_GetImageId(void)
{
  return (HAL_RTCEx_BKUPRead(&RtcHandle, BKUP_IMAGE_ID));
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

