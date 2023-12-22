/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    TIM/TIM_EncoderIndex_PulseOnCompare/Src/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* Compute the prescaler value to have emulation timer counter clock equal to 1 MHz */
#define EMU_PRESCALER     ((SystemCoreClock / 1000000) - 1)

/* Compute the emulation period value to have encoder clock equal to 1 kHz */
/* Divide by 2 due to Toggle output mode. Multiply by 4 due to both edges of 2 channels */
#define EMU_PERIOD        (((SystemCoreClock/(EMU_PRESCALER + 1)*4/2)/1000) - 1)
#define PULSE1_VALUE      ((EMU_PERIOD * 1 )/4)
#define PULSE2_VALUE      ((EMU_PERIOD * 3 )/4)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
