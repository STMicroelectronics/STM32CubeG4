/**
  ******************************************************************************
  * @file           : BuckBoost.h
  * @brief          : Header for BuckBoost.c file.
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

#undef GLOBAL
#ifdef __BUCKBOOST_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUCKBOOST_H
#define __BUCKBOOST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include "HW_3p3z_controller.h"
  
/* Exported types ------------------------------------------------------------*/
typedef enum {
  MANUAL = 0u,
  AUTOMATIC = 1u
} Mode_TypeDef;

typedef struct G4_Demo_t
{
  FMAC_FilterConfigTypeDef sFmacConfig; // FMAC Filter settings
  uint32_t periodBuck;    // Number of ticks per period, read from hrtim register
  uint32_t periodBoost;   // Number of ticks per period, read from hrtim register
  uint8_t NbrActiveLoad;        // Memorizes the number of active Resistors load
  Mode_TypeDef Mode;      // Indicates if loads are manually switched or not
} G4_Demo_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private functions prototypes ----------------------------------------------*/
uint16_t HAL_HRTIM_NsToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int ns);
uint16_t HAL_HRTIM_FreqToTicks(HRTIM_HandleTypeDef *hhrtim, uint32_t Timers, int FreqHz);
void CheckInputVoltage(void);
void BuckBoost_LoadSelection(void);

/* Exported functions prototypes ---------------------------------------------*/
GLOBAL void Demo_Buck(void);
GLOBAL void Demo_BuckBoost(void);
static const float scaleTable[] = { 32, 16, 8, 4, 2, 1, 1.0f/2, 1.0f/4 };
GLOBAL G4_Demo_t Demo;

/* Private defines -----------------------------------------------------------*/
/**************************************************/
/* BuckBoost pins definitions                     */
/**************************************************/
#define BUCKBOOST_USBPD_EN_Pin          GPIO_PIN_3
#define BUCKBOOST_USBPD_EN_GPIO_Port    GPIOC
#define BUCKBOOST_LOAD_1_Pin            GPIO_PIN_14
#define BUCKBOOST_LOAD_1_GPIO_Port      GPIOC
#define BUCKBOOST_LOAD_2_Pin            GPIO_PIN_15
#define BUCKBOOST_LOAD_2_GPIO_Port      GPIOC
#define BUCKBOOST_VIN_Pin               GPIO_PIN_1
#define BUCKBOOST_VIN_GPIO_Port         GPIOA
#define BUCKBOOST_I_IN_AVG_Pin          GPIO_PIN_2
#define BUCKBOOST_I_IN_AVG_GPIO_Port    GPIOA
#define BUCKBOOST_VOUT_Pin              GPIO_PIN_3
#define BUCKBOOST_VOUT_GPIO_Port        GPIOA
#define BUCKBOOST_I_IN_SENSE_Pin        GPIO_PIN_11
#define BUCKBOOST_I_IN_SENSE_GPIO_Port  GPIOB
#define BUCKBOOST_P1_DRIVE_Pin          GPIO_PIN_12
#define BUCKBOOST_P1_DRIVE_GPIO_Port    GPIOB
#define BUCKBOOST_N1_DRIVE_Pin          GPIO_PIN_13
#define BUCKBOOST_N1_DRIVE_GPIO_Port    GPIOB
#define BUCKBOOST_N2_DRIVE_Pin          GPIO_PIN_14
#define BUCKBOOST_N2_DRIVE_GPIO_Port    GPIOB
#define BUCKBOOST_P2_DRIVE_Pin          GPIO_PIN_15
#define BUCKBOOST_P2_DRIVE_GPIO_Port    GPIOB
#define GPO1_Pin                        GPIO_PIN_9
#define GPO1_GPIO_Port                  GPIOB

/**************************************************/
/* Paste coefficients from Biricha WDS hereafter  */
/**************************************************/
#define SwitchFreqBoost 200e3f // Buck freq. or Boost part when used in Buck&Boost mode
#define SwitchFreqBuck  400e3f // Buck part when used in Buck&Boost mode

#define HRTIM_MAX_DUTY  24480   // Max allowed duty when using hi-res PWM
#define HRTIM_MIN_DUTY  0x60    // Min allowed duty when using hi-res PWM
#define fHRTIM_HZ       170e6   // Internal clock frequency usage

//#define DSMPS_DEBUG             // Used for Digital Power debug purpose

#ifdef __cplusplus
}
#endif

#endif /* __BUCKBOOST_H */


