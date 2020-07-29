/**
  ******************************************************************************
  * @file    Adafruit_LCD_1_8_SD_Joystick/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

