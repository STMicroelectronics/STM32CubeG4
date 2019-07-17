/**
  ******************************************************************************
  * @file    Demonstrations/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#undef GLOBAL
#ifdef __MAIN_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "b_g474e_dpow1_conf.h"
#include "b_g474e_dpow1.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  STATE_4LED = 0,
  STATE_LED_COLOR,
  STATE_LED_WHITE,
  STATE_BUCK,
  STATE_BUCKBOOST,
  MAX_STATE
} STATE_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------*/
/* Demo state machine */
GLOBAL __IO  STATE_TypeDef  StateMachine;

/* Detection of keypress */
GLOBAL __IO uint8_t UserAction;
GLOBAL __IO uint8_t UserEvent;

/* Triangle generation compare value */
GLOBAL __IO uint16_t TriangCmp;
GLOBAL HRTIM_HandleTypeDef  HrtimHandle;

/* Exported functions ------------------------------------------------------- */
GLOBAL void Error_Handler(void);
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
