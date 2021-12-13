/**
  ******************************************************************************
  * @file    Demonstrations/Inc/main.h
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
