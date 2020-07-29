/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd.c
  * @author  MCD Application Team
  * @brief   This file contains the device define.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbpd.h"
#include "gui_api.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */

/* Private variables ---------------------------------------------------------*/
const uint8_t HWBoardVersionName[] = "STM32G474E-EVAL";
const uint8_t PDTypeName[] = "MB1397B";

/* Private functions ---------------------------------------------------------*/
static const uint8_t*          GetHWBoardVersionName(void);
static const uint8_t*          GetPDTypeName(void);

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
  GUI_Init(GetHWBoardVersionName, GetPDTypeName, HW_IF_PWR_GetVoltage, HW_IF_PWR_GetCurrent);

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
  * @brief  This method returns HW board version name
  * @retval HW Board version name
  */
static const uint8_t* GetHWBoardVersionName(void)
{
  return HWBoardVersionName;
}

/**
  * @brief  This method returns HW PD Type name
  * @retval HW Board version name
  */
static const uint8_t* GetPDTypeName(void)
{
  return PDTypeName;
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
