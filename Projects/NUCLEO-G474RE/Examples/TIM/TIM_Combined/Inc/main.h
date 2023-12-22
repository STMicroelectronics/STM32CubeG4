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
/* Generate 3 combined PWM signals:
	TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)    
	=> TIM1CLK = PCLK2 = SystemCoreClock
	TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	SystemCoreClock is set to 170 MHz for STM32G474RETx devices

	The objective is to generate 3 combined PWM signal at 8.78 KHz (in center aligned mode):
	 - TIM1_Period = (SystemCoreClock / (8.78*2))
	The channel 1  duty cycle is set to 50%
	The channel 2  duty cycle is set to 37.5%
	The channel 3  duty cycle is set to 25%
	The Timer pulse is calculated as follows in center aligned mode:
	 - ChannelxPulse = (DutyCycle * TIM1_Period) / 100
		  
	The channel 5  is used in PWM2 mode with duty cycle set to 6.22%
	   
	The 3 resulting signals are made of an AND logical combination of two reference PWMs:
	- Channel 1 and Channel 5
	- Channel 2 and Channel 5
	- Channel 3 and Channel 5

	Note: 
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32g4xx.c file.
	 Each time the core clock (HCLK) changes, user had to update SystemCoreClock 
	 variable value. Otherwise, any configuration based on this variable will be incorrect.
	 This variable is updated in three ways:
	  1) by calling CMSIS function SystemCoreClockUpdate()
	  2) by calling HAL API function HAL_RCC_GetSysClockFreq()
	  3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency     
*/
/* Compute the value to be set in ARR register to generate signal frequency at 8.78 Khz */
 
#define PERIOD_VALUE  ((SystemCoreClock) / 17570 ) -1
/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 */
#define  PULSE_CH1_VALUE (uint16_t) ((uint32_t) (5 * (PERIOD_VALUE-1)) / 10) 
/* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 */
#define  PULSE_CH2_VALUE (uint16_t) ((uint32_t) (375 * (PERIOD_VALUE-1)) / 1000)
/* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 */
#define  PULSE_CH3_VALUE (uint16_t) ((uint32_t) (25 * (PERIOD_VALUE-1) ) / 100)
/* Compute CCR5 value to generate a duty cycle at 6.22%  for channel 5 (in PWM2)*/
#define  PULSE_CH5_VALUE (uint16_t) ((uint32_t) (622 * (PERIOD_VALUE-1)) / 10000)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
