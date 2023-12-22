/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           main.h
  * @brief          This file contains the common defines of the application
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
#define WAVEFORM_RAMP_12BITS_5SAMPLES  ((uint32_t)   5)    /* Size of waveform Waveform_Ramp_12bits_5samples[] */
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint32_t)  30)    /* Size of array aADCxConvertedValues[] */
#define ADC_MEASUREMENT_TOLERANCE_LSB  ((uint32_t) 140)    /* ADC measurement tolerance of uncertainty (unit: LSB). Note: higher than theoretical tolerance because board routing and connectors are not designed for optimal ADC performance. */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
