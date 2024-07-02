/**
  ******************************************************************************
  * @file    BSP/Src/log.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the LCD Log firmware functions
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lcd_log.h"

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LCD Log demo
  * @param  None
  * @retval None
  */
void Log_demo(void)
{
  JOYState_TypeDef JoyState = JOY_NONE;
  uint8_t i = 0;

  /* Wait For User inputs */
  while(CheckForUserInput() == 0);

  BSP_JOY_Init(JOY_MODE_GPIO);

  /* Initialize LCD Log module */
  LCD_LOG_Init();

  /* Show Header and Footer texts */
  LCD_LOG_SetHeader((uint8_t *)"Log Example");
  LCD_LOG_SetFooter((uint8_t *)"Use Joystick to scroll up/down");

  /* Output User logs */
  for (i = 0; i < 35; i++)
  {
    LCD_UsrLog ("This is Line %d \n", i);
  }

  HAL_Delay(2000);

   /* Clear Old logs */
  LCD_LOG_ClearTextZone();

   /* Output new user logs */
  for (i = 0; i < 70; i++)
  {
    LCD_UsrLog ("This is Line %d \n", i);
  }

  /* Check for joystick user input for scroll (back and forward) */
  while (1)
  {
    JoyState = BSP_JOY_GetState();
    switch(JoyState)
    {
    case JOY_UP:
      LCD_LOG_ScrollBack();
      break;
    case JOY_DOWN:
      LCD_LOG_ScrollForward();
      break;

    default:
      break;
    }
    if(CheckForUserInput() > 0)
    {
      return;
    }
    HAL_Delay (10);
  }
}

/**
  * @}
  */

/**
  * @}
  */


