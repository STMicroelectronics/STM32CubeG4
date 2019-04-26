/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#include "stm32g4xx_hal.h"
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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PF0_OSC_IN_Pin GPIO_PIN_0
#define PF0_OSC_IN_GPIO_Port GPIOF
#define PF1_OSC_OUT_Pin GPIO_PIN_1
#define PF1_OSC_OUT_GPIO_Port GPIOF
#define B_USER_Pin GPIO_PIN_0
#define B_USER_GPIO_Port GPIOA
#define Vin_Sense_Pin GPIO_PIN_1
#define Vin_Sense_GPIO_Port GPIOA
#define Vout_Sense_Pin GPIO_PIN_3
#define Vout_Sense_GPIO_Port GPIOA
#define BK_Drive_Pin GPIO_PIN_12
#define BK_Drive_GPIO_Port GPIOB
#define RC_Pin GPIO_PIN_14
#define RC_GPIO_Port GPIOB
#define P1_Drive_Pin GPIO_PIN_8
#define P1_Drive_GPIO_Port GPIOA
#define N1_Drive_Pin GPIO_PIN_9
#define N1_Drive_GPIO_Port GPIOA
#define N2_Drive_Pin GPIO_PIN_10
#define N2_Drive_GPIO_Port GPIOA
#define P2_Drive_Pin GPIO_PIN_11
#define P2_Drive_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define LD_U_Pin GPIO_PIN_6
#define LD_U_GPIO_Port GPIOB
#define LD_D_Pin GPIO_PIN_7
#define LD_D_GPIO_Port GPIOB
#define LD_L_Pin GPIO_PIN_8
#define LD_L_GPIO_Port GPIOB
#define LD_R_Pin GPIO_PIN_9
#define LD_R_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
