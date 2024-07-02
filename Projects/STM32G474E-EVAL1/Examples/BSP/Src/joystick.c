/**
  ******************************************************************************
  * @file    BSP/Src/joystick.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the joystick feature in the
  *          STM32G474E EVAL1 driver
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


#if (USE_BSP_IO_CLASS == 1)
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

__IO JOYPin_TypeDef JoyPinPressed = JOY_NONE;
uint32_t JoyStickDemo = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  JOY demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Joy_demo(void)
{
  int32_t result  = 0;

  printf("TEST OF JOYSTICK\n");
  printf("                  mode GPIO\n");

  /*********************/
  /* Test in GPIO mode */
  /*********************/
  /* Initialize joystick */
  if (BSP_JOY_Init(JOY1, JOY_MODE_GPIO, JOY_ALL) != BSP_ERROR_NONE) result--;

  /* Test of BSP_JOY_GetState function */
  printf("Please press JOY_SEL\n");
  while (BSP_JOY_GetState(JOY1) != JOY_SEL);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */

  printf("Please press JOY_UP\n");
  while (BSP_JOY_GetState(JOY1) != JOY_UP);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */

  printf("Please press JOY_DOWN\n");
  while (BSP_JOY_GetState(JOY1) != JOY_DOWN);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */

  printf("Please press JOY_RIGHT\n");
  while (BSP_JOY_GetState(JOY1) != JOY_RIGHT);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */

  printf("Please press JOY_LEFT\n");
  while (BSP_JOY_GetState(JOY1) != JOY_LEFT);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */

  /* De-Initialize joystick */
  if (BSP_JOY_DeInit(JOY1, JOY_ALL) != BSP_ERROR_NONE) result--;

  /*********************/
  /* Test in EXTI mode */
  /*********************/
  printf("                  mode EXTI (IRQ)\n");
  /* Initialize joystick */
  if (BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL) != BSP_ERROR_NONE) result--;

  printf("Please press JOY_SEL\n");
  while (JoyPinPressed != JOY_SEL);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */
  JoyPinPressed = JOY_NONE;

  printf("Please press JOY_UP\n");
  while (JoyPinPressed != JOY_UP);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */
  JoyPinPressed = JOY_NONE;

  printf("Please press JOY_DOWN\n");
  while (JoyPinPressed != JOY_DOWN);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */
  JoyPinPressed = JOY_NONE;

  printf("Please press JOY_RIGHT\n");
  while (JoyPinPressed != JOY_RIGHT);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */
  JoyPinPressed = JOY_NONE;

  printf("Please press JOY_LEFT\n");
  while (JoyPinPressed != JOY_LEFT);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */
  JoyPinPressed = JOY_NONE;

  /* De-Initialize joystick */
  if (BSP_JOY_DeInit(JOY1, JOY_ALL) != BSP_ERROR_NONE) result--;

  if (result == 0)   printf("      Passed\n");
        else         printf("      Failed\n");

  return result;
}

void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  JoyPinPressed = JoyPin;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* #if (USE_BSP_IO_CLASS == 1) */

