/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_DualBoot/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "stm32_lcd.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* Select Bank to be flashed */
#define FLASH_BANK1
/* #define FLASH_BANK2 */

#if defined(FLASH_BANK1)&&defined(FLASH_BANK2)
  #error "Select either FLASH_BANK1 or FLASH_BANK2"
#endif

/* Messages to display on LCD */
#define MESSAGE1     "STM32G474QET6"
#define MESSAGE2     "Flash Dual-boot example"
#define MESSAGE3     "Device running on"
#ifdef FLASH_BANK1
#define MESSAGE4     "FLASH BANK1"
#else
#define MESSAGE4     "FLASH BANK2"
#endif
#define MESSAGE5     "Push User push-button"
#define MESSAGE6     "to swap bank"


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
