/**
  ******************************************************************************
  * @file    BSP/Src/led.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the led feature in the
  *          STM32G474E EVAL1 driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
  * @brief  Led demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Led_demo(void)
{
  int32_t  result = 0;

  printf("TEST OF LEDs\n");

  /* Initialize the LEDs */
  if (BSP_LED_Init(LED1) != BSP_ERROR_NONE) result--;
  if (BSP_LED_Init(LED3) != BSP_ERROR_NONE) result--;
#if (USE_BSP_IO_CLASS == 1)
  if (BSP_LED_Init(LED2) != BSP_ERROR_NONE) result--;
  if (BSP_LED_Init(LED4) != BSP_ERROR_NONE) result--;
#endif

  /* Switch on the LEDs */
  printf("Switch on LED1\n");
  if (BSP_LED_On(LED1) != BSP_ERROR_NONE) result--;
  printf("Switch on LED3\n");
  if (BSP_LED_On(LED3) != BSP_ERROR_NONE) result--;

#if (USE_BSP_IO_CLASS == 1)
  printf("Switch on LED2\n");
  if (BSP_LED_On(LED2) != BSP_ERROR_NONE) result--;
  printf("Switch on LED4\n");
  if (BSP_LED_On(LED4) != BSP_ERROR_NONE) result--;
#endif
  /* Check LEDs states */
  if (BSP_LED_GetState(LED1) != 1) result--;
  if (BSP_LED_GetState(LED3) != 1) result--;
#if (USE_BSP_IO_CLASS == 1)
  if (BSP_LED_GetState(LED2) != 1) result--;
  if (BSP_LED_GetState(LED4) != 1) result--;
#endif

  /* Check result */
  if (CheckResult() != 0U) result--;

  /* Toggle the LEDs */
  printf("Switch off LED1\n");
  if (BSP_LED_Toggle(LED1) != BSP_ERROR_NONE) result--;
  printf("Switch off LED3\n");
  if (BSP_LED_Toggle(LED3) != BSP_ERROR_NONE) result--;
#if (USE_BSP_IO_CLASS == 1)
  printf("Switch off LED2\n");
  if (BSP_LED_Toggle(LED2) != BSP_ERROR_NONE) result--;
  printf("Switch off LED4\n");
  if (BSP_LED_Toggle(LED4) != BSP_ERROR_NONE) result--;
#endif

  /* Check LEDs states */
  if (BSP_LED_GetState(LED1) != 0) result--;
  if (BSP_LED_GetState(LED3) != 0) result--;
#if (USE_BSP_IO_CLASS == 1)
  if (BSP_LED_GetState(LED2) != 0) result--;
  if (BSP_LED_GetState(LED4) != 0) result--;
#endif

  /* Check result */
  if (CheckResult() != 0U) result--;

  /* Toggle the LEDs */
  printf("Switch on LED1\n");
  if (BSP_LED_Toggle(LED1) != BSP_ERROR_NONE) result--;
  printf("Switch on LED3\n");
  if (BSP_LED_Toggle(LED3) != BSP_ERROR_NONE) result--;
#if (USE_BSP_IO_CLASS == 1)
  printf("Switch on LED2\n");
  if (BSP_LED_Toggle(LED2) != BSP_ERROR_NONE) result--;
  printf("Switch on LED4\n");
  if (BSP_LED_Toggle(LED4) != BSP_ERROR_NONE) result--;
#endif

  /* Check result */
  if (CheckResult() != 0U) result--;

  /* Switch off the LEDs */
  printf("Switch off LED1\n");
  if (BSP_LED_Off(LED1) != BSP_ERROR_NONE) result--;
  printf("Switch off LED3\n");
  if (BSP_LED_Off(LED3) != BSP_ERROR_NONE) result--;
#if (USE_BSP_IO_CLASS == 1)
  printf("Switch off LED2\n");
  if (BSP_LED_Off(LED2) != BSP_ERROR_NONE) result--;
  printf("Switch off LED4\n");
  if (BSP_LED_Off(LED4) != BSP_ERROR_NONE) result--;
#endif

  /* Check result */
  if (CheckResult() != 0U) result--;

  /* De-Initialize the LEDs */
  if (BSP_LED_DeInit(LED1) != BSP_ERROR_NONE) result--;
  if (BSP_LED_DeInit(LED3) != BSP_ERROR_NONE) result--;
#if (USE_BSP_IO_CLASS == 1)
  if (BSP_LED_DeInit(LED2) != BSP_ERROR_NONE) result--;
  if (BSP_LED_DeInit(LED4) != BSP_ERROR_NONE) result--;
#endif

#if (USE_BSP_IO_CLASS == 1)
  /* Add a call of BSP_IO_DeInit for a clean exit of this test */
  if (BSP_IO_DeInit(0) != BSP_ERROR_NONE) result--;
#endif

  return result;
}

/**
  * @}
  */

/**
  * @}
  */


