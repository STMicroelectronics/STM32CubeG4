/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd.c
  * @author  MCD Application Team
  * @brief   This file contains the device define.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbpd.h"
#include "gui_api.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* Private variables ---------------------------------------------------------*/
#define BSP_BOARD_NAME  "STM32G474E-EVAL";
#define BSP_BOARD_ID    "MB1397B";

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN 2 */
/* USER CODE END 2 */
/* USBPD init function */
void MX_USBPD_Init(void)
{

  /* Global Init of USBPD HW */
  USBPD_HW_IF_GlobalHwInit();

  /* Initialize the Device Policy Manager */
  if (USBPD_OK != USBPD_DPM_InitCore())
  {
    while(1);
  }

  /* Initialize GUI before retrieving PDO from RAM */
  GUI_Init(BSP_GetBoardName, BSP_GetBoardID, HW_IF_PWR_GetVoltage, HW_IF_PWR_GetCurrent);

  /* Initialise the DPM application */
  if (USBPD_OK != USBPD_DPM_UserInit())
  {
    while(1);
  }

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */

  if (USBPD_OK != USBPD_DPM_InitOS())
  {
    while(1);
  }

  /* USER CODE BEGIN EnableIRQ */
  /* Enable IRQ which has been disabled by FreeRTOS services */
  __enable_irq();
  /* USER CODE END EnableIRQ */

}

/**
  * @brief  This method returns the board name
  * @retval pointer to the board name string
  */
__weak const uint8_t* BSP_GetBoardName(void)
{
  return (const uint8_t*)BSP_BOARD_NAME;
}

/**
  * @brief  This method returns the board ID
  * @retval pointer to the board ID string
  */
__weak const uint8_t* BSP_GetBoardID(void)
{
  return (const uint8_t*)BSP_BOARD_ID;
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @}
  */

/**
  * @}
  */
