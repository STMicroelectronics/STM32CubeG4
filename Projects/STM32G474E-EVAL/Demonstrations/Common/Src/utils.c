/**
  ******************************************************************************
  * @file    utils.c
  * @author  MCD Application Team
  * @brief   STM32G474E_EVAL demonstration firmware utilities
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



