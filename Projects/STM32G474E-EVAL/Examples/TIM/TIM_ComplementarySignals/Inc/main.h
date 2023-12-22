/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/*
    Generate 3 complementary PWM signals with 3 different duty cycles:
    TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2), since APB2
    prescaler is 1.
    TIM1CLK = PCLK2
    PCLK2 = HCLK
    => TIM1CLK = HCLK = SystemCoreClock
  
    TIM1CLK is fixed to SystemCoreClock, the TIM1 Prescaler is set to have
    TIM1 counter clock = 10MHz.

    The objective is to generate PWM signal at 10 KHz:
    - TIM1_Period = (TIM1 counter clock / 10000) - 1

    The Three Duty cycles are computed as the following description: 
    - The channel 1 duty cycle is set to 50% so channel 1N is set to 50%.
    - The channel 2 duty cycle is set to 25% so channel 2N is set to 75%.
    - The channel 3 duty cycle is set to 12.5% so channel 3N is set to 87.5%.
    
    The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
          
  Compute the prescaler value to have TIM1 counter clock equal to 10MHz 
    - Prescaler = (SystemCoreClock/10000000) - 1
  */
#define  PRESCALER_VALUE    (uint32_t) ((SystemCoreClock  / 10000000) - 1)  
#define  PERIOD_VALUE       (1000-1)  /* Period Value  */
#define  PULSE1_VALUE       500         /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       250         /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       125         /* Capture Compare 3 Value  */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
