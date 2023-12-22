/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         main.h
  * @author       STMicroelectronics
  * @version      V1.0.0
  * @date         06-Jan-2022
  * @brief        Header for main.c file
  * @details      This file contains the common defines and includes for the 
  *               application
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  *
 * @history Date       Version Person  Change
 * @history ---------- ------- ------- ----------------------------------------
  * @history 2022-01-06 1.0     NSa    First version of the file
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
#include "stm32g4xx_ll_ucpd.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_dma.h"

#include "stm32g4xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define ZCD_PULSE_WIDTH 300
#define OC_PULSE_WIDTH 300

//Typical average PWM frequencies: 80kHz --> 12.5us
//Typical TONmin = 400ns
//HRTIM Freq = 2.72GHz --> Per = 368ps

#define TON_MAX  30000   // 11.1us
#define TOFF_MAX 30000   // 11.1us
#define TON_MIN  1000    // 367 ns
#define TOFF_MIN 6000    // 2205 ns

#define MAX_DUTY_CYCLE 0xFFF6

#define EVENT_DELTA_WHEN_BUTTON_PRESSED 5

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

void HAL_HRTIM_MspPostInit(HRTIM_HandleTypeDef *hhrtim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define JOYSTICK_SEL_Pin GPIO_PIN_13
#define JOYSTICK_SEL_GPIO_Port GPIOC
#define JOYSTICK_SEL_EXTI_IRQn EXTI15_10_IRQn
#define BUCKBOOST_LOAD_1_Pin GPIO_PIN_14
#define BUCKBOOST_LOAD_1_GPIO_Port GPIOC
#define BUCKBOOST_LOAD_2_Pin GPIO_PIN_15
#define BUCKBOOST_LOAD_2_GPIO_Port GPIOC
#define IT_TIMEFRAME_Pin GPIO_PIN_1
#define IT_TIMEFRAME_GPIO_Port GPIOF
#define USBPD_VIN_Pin GPIO_PIN_2
#define USBPD_VIN_GPIO_Port GPIOC
#define BUCKBOOST_USBPD_EN_Pin GPIO_PIN_3
#define BUCKBOOST_USBPD_EN_GPIO_Port GPIOC
#define BUCK_GREEN_SENSE_Pin GPIO_PIN_0
#define BUCK_GREEN_SENSE_GPIO_Port GPIOA
#define BUCKBOOST_VIN_Pin GPIO_PIN_1
#define BUCKBOOST_VIN_GPIO_Port GPIOA
#define BUCKBOOST_I_IN_AVG_Pin GPIO_PIN_2
#define BUCKBOOST_I_IN_AVG_GPIO_Port GPIOA
#define BUCKBOOST_VOUT_Pin GPIO_PIN_3
#define BUCKBOOST_VOUT_GPIO_Port GPIOA
#define BUCK_RED_SENSE_Pin GPIO_PIN_7
#define BUCK_RED_SENSE_GPIO_Port GPIOA
#define JOYSTICK_LEFT_Pin GPIO_PIN_4
#define JOYSTICK_LEFT_GPIO_Port GPIOC
#define JOYSTICK_DOWN_Pin GPIO_PIN_5
#define JOYSTICK_DOWN_GPIO_Port GPIOC
#define BUCK_BLUE_SENSE_Pin GPIO_PIN_0
#define BUCK_BLUE_SENSE_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_1
#define LD3_GPIO_Port GPIOB
#define JOYSTICK_RIGHT_Pin GPIO_PIN_2
#define JOYSTICK_RIGHT_GPIO_Port GPIOB
#define JOYSTICK_UP_Pin GPIO_PIN_10
#define JOYSTICK_UP_GPIO_Port GPIOB
#define JOYSTICK_UP_EXTI_IRQn EXTI15_10_IRQn
#define BUCKBOOST_I_IN_SENSE_Pin GPIO_PIN_11
#define BUCKBOOST_I_IN_SENSE_GPIO_Port GPIOB
#define BUCKBOOST_P1_DRIVE_Pin GPIO_PIN_12
#define BUCKBOOST_P1_DRIVE_GPIO_Port GPIOB
#define BLANKING_WINDOWS_Pin GPIO_PIN_13
#define BLANKING_WINDOWS_GPIO_Port GPIOB
#define PFC_PWM_Pin GPIO_PIN_14
#define PFC_PWM_GPIO_Port GPIOB
#define BUCKBOOST_P2_DRIVE_Pin GPIO_PIN_15
#define BUCKBOOST_P2_DRIVE_GPIO_Port GPIOB
#define OC_out_Pin GPIO_PIN_7
#define OC_out_GPIO_Port GPIOC
#define BUCK_GREEN_DRIVE_Pin GPIO_PIN_8
#define BUCK_GREEN_DRIVE_GPIO_Port GPIOC
#define ZCD_out_Pin GPIO_PIN_9
#define ZCD_out_GPIO_Port GPIOC
#define BUCK_BLUE_DRIVE_Pin GPIO_PIN_8
#define BUCK_BLUE_DRIVE_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_15
#define LD2_GPIO_Port GPIOA
#define USART3_TX_Pin GPIO_PIN_10
#define USART3_TX_GPIO_Port GPIOC
#define USART3_RX_Pin GPIO_PIN_11
#define USART3_RX_GPIO_Port GPIOC
#define USBPD_1A_PROTECT_Pin GPIO_PIN_12
#define USBPD_1A_PROTECT_GPIO_Port GPIOC
#define USBPD_550mA_PROTECT_Pin GPIO_PIN_2
#define USBPD_550mA_PROTECT_GPIO_Port GPIOD
#define ZCD_in_Pin GPIO_PIN_5
#define ZCD_in_GPIO_Port GPIOB
#define LD4_Pin GPIO_PIN_7
#define LD4_GPIO_Port GPIOB
#define OC_in_Pin GPIO_PIN_9
#define OC_in_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
