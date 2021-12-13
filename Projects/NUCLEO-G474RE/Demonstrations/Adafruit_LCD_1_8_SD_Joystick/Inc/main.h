/**
  ******************************************************************************
  * @file    Adafruit_LCD_1_8_SD_Joystick/Inc/main.h
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
#include "stm32g4xx_nucleo.h"
#include "adafruit_802.h"
#include "adafruit_802_sd.h"
#include "adafruit_802_lcd.h"
#include "stm32_lcd.h"
#include <stdio.h>
#include <stdlib.h>


/* Exported types ------------------------------------------------------------*/
typedef struct
{
  void   (*DemoFunc)(void);
  uint8_t DemoName[30];
  uint32_t DemoIndex;
}BSP_DemoTypedef;

extern const unsigned char stlogo[];
extern __IO uint32_t ButtonState;
/* Exported constants --------------------------------------------------------*/
#define KEY_NOT_PRESSED 0x00
#define KEY_PRESSED     0x01

/* TIM2 Autoreload and Capture Compare register values */
/* Exported macro ------------------------------------------------------------*/

#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))

extern EXTI_HandleTypeDef           hpb_exti[];
extern uint32_t JoyState;
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
uint8_t CheckForUserInput(void);

void Joystick_demo (void);
void LCD_demo (void);
void SD_demo (void);


#endif /* MAIN_H */



