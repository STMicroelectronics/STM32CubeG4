/**
  ******************************************************************************
  * @file    bsp_gui.h
  * @author  MCD Application Team
  * @brief   This file contains bsp interface control functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef BSP_GUI_H
#define BSP_GUI_H
/* Includes ------------------------------------------------------------------*/
#include "usbpd_gui_memmap.h"
#include "gui_api.h"
#if defined(_VDM)
#include "usbpd_vdm_user.h"
#endif /* _VDM */
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  GUI_OK,
  GUI_ERASE_ERROR,
  GUI_WRITE_ERROR,
  GUI_ERROR
} GUI_StatusTypeDef;

/* Variable containing ADC conversions results */
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Load User settings from FLASH if GUI saving area is not empty (0xFFFFFFFF)
  * @retval GUI status state (GUI_OK: USER Settings from GUI saving area else from original FW)
  */
GUI_StatusTypeDef     BSP_GUI_LoadDataFromFlash(void);

/**
  * @brief  Save User settings in GUI saving area
  * @retval GUI status state
  */
GUI_StatusTypeDef     BSP_GUI_SaveDataInFlash(void);

#endif /* BSP_GUI_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

