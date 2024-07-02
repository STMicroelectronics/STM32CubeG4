/**
  ******************************************************************************
  * @file    BSP/Src/potentiometer.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the potentiometer feature in the
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
  * @brief  Potentiometer demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Pot_demo(void)
{
  int32_t  result = 0;
  int32_t  PotValue;
  uint32_t i;

  printf("TEST OF POTENTIOMETER\n");

  printf("Please put JP5 in position 1-2 (POT))\n");

  StartTest();

  /* Initialize potentiometer */
  if (BSP_POT_Init(POT1) != BSP_ERROR_NONE) result--;

  printf("Please turn potentiometer button (10 measures will be performed)\n");

  for (i = 0; i < 10; i++)
  {
    /* Get potentiometer value */
    PotValue = BSP_POT_GetLevel(POT1);

    /* Display value or error */
    if (PotValue < 0)
    {
      printf("Error returned by BSP_POT_GetLevel\n");
      result--;
    }
    else
    {
      printf("Potentiometer value is %ld percent\n", PotValue);
    }

    /* Wait 1 second for next measure */
    HAL_Delay(1000);
  }

  /* De-initialize potentiometer */
  if (BSP_POT_DeInit(POT1) != BSP_ERROR_NONE) result--;

  return result;
}

/**
  * @}
  */

/**
  * @}
  */


