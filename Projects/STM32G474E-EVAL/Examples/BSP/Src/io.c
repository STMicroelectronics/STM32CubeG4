/**
  ******************************************************************************
  * @file    BSP/Src/io.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the IO feature in the
  *          STM32G474E EVAL driver
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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  IO demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Io_demo(void)
{
  int32_t       result  = 0;
  BSP_IO_Init_t Init;

  printf("TEST OF IO\n");

  /*****************/
  /* Initialize IO */
  /*****************/
  Init.Pin  = IO_PIN_7;
  Init.Mode = IO_MODE_OUTPUT_PP;
  Init.Pull = IO_PULLUP;
  /* Test with wrong parameters */
  if (BSP_IO_Init(1, &Init) != BSP_ERROR_WRONG_PARAM) result--;
  if (BSP_IO_Init(0, NULL) != BSP_ERROR_WRONG_PARAM) result--;

  /* Test with correct parameters */
  if (BSP_IO_Init(0, &Init) != BSP_ERROR_NONE) result--; /* LED6 */
  Init.Pin  = IO_PIN_0;
  Init.Mode = IO_MODE_IT_LOW_LEVEL;
  Init.Pull = IO_PULLUP;
  if (BSP_IO_Init(0, &Init) != BSP_ERROR_NONE) result--; /* JOY_SEL */

  /**************************/
  /* Write, read, toggle IO */
  /**************************/
  /* Test with wrong parameters */
  if (BSP_IO_WritePin(1, IO_PIN_7, IO_PIN_RESET) != BSP_ERROR_WRONG_PARAM) result--;
  if (BSP_IO_ReadPin(1, IO_PIN_7) != BSP_ERROR_WRONG_PARAM) result--;
  if (BSP_IO_TogglePin(1, IO_PIN_7) != BSP_ERROR_WRONG_PARAM) result--;

  /* Test with correct parameters */
  if (BSP_IO_WritePin(0, IO_PIN_7, IO_PIN_RESET) != BSP_ERROR_NONE) result--;
  printf("Check that LED4 is ON\n");
  if (CheckResult() != 0U) result--;
  if (BSP_IO_ReadPin(0, IO_PIN_7) != IO_PIN_RESET) result--;

  if (BSP_IO_TogglePin(0, IO_PIN_7) != BSP_ERROR_NONE) result--;
  printf("Check that LED4 is OFF\n");
  if (CheckResult() != 0U) result--;
  if (BSP_IO_ReadPin(0, IO_PIN_7) != IO_PIN_SET) result--;

  /********************/
  /* Get and clear IT */
  /********************/
  /* Test with wrong parameters */
  if (BSP_IO_GetIT(1, IO_PIN_0) != BSP_ERROR_WRONG_PARAM) result--;
  if (BSP_IO_ClearIT(1, IO_PIN_0) != BSP_ERROR_WRONG_PARAM) result--;

  /* Test with correct parameters */
  if (BSP_IO_GetIT(0, IO_PIN_0) != IO_PIN_IT_RESET) result--;
  printf("Please press JOY_SEL\n");
  while (BSP_IO_GetIT(0, IO_PIN_0) != IO_PIN_IT_SET);
  printf("OK\n");
  HAL_Delay(1000); /* Debounce time */
  if (BSP_IO_ClearIT(0, IO_PIN_0) != BSP_ERROR_NONE) result--;
  if (BSP_IO_GetIT(0, IO_PIN_0) != IO_PIN_IT_RESET) result--;

  /********************/
  /* De-Initialize IO */
  /* Test with wrong parameters */
  if (BSP_IO_DeInit(1) != BSP_ERROR_WRONG_PARAM) result--;

  /* Test with correct parameters */
  if (BSP_IO_DeInit(0) != BSP_ERROR_NONE) result--;

  if (result==0)   printf("      Passed\n");
        else    printf("      Failed\n");

  return result;
}


/**
  * @}
  */

/**
  * @}
  */

#endif /* #if (USE_BSP_IO_CLASS == 1) */


