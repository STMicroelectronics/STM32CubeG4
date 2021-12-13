/**
  ******************************************************************************
  * @file    Examples/BSP/Inc/main.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stm32g4xx_hal.h"
#include "b_g474e_dpow1_errno.h"
#include "b_g474e_dpow1_conf.h"
#include "b_g474e_dpow1.h"

/* Exported variables --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  int32_t  (*DemoFunc)(void);
  uint8_t  DemoName[50];
} BSP_DemoTypedef;

/* Exported constants --------------------------------------------------------*/
#define KEYS_NUMBER    JOY_KEY_NUMBER
#define DEBOUNCE 100
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
int32_t  Joy_demo(void);
int32_t  Led_demo(void);

void     Error_Handler(void);
uint32_t CheckResult(void);
void     StartTest(void);
uint8_t  CheckForUserInput(void);

void Error_Handler(void);

#endif /* MAIN_H */


