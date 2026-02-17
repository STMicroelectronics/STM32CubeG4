/**
  ******************************************************************************
  * @file    bsp_gui.h
  * @author  MCD Application Team
  * @brief   This file contains bsp interface control functions.
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
  * @brief  Erase User settings in GUI saving area
  * @retval GUI status state
  */
GUI_StatusTypeDef BSP_GUI_EraseDataInFlash(void);

/**
  * @brief  Save User settings in GUI saving area
  * @retval GUI status state
  */
GUI_StatusTypeDef     BSP_GUI_SaveDataInFlash(void);

#endif /* BSP_GUI_H */

