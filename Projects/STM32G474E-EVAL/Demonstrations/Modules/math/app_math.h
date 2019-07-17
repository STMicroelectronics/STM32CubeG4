/**
  ******************************************************************************
  * @file    app_whiteled.h
  * @author  MCD Application Team
  * @brief   White led application interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
#undef GLOBAL
#ifdef __APP_MATH_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "k_module.h"

/* External constants --------------------------------------------------------*/
#define TRUE 0x01
#define FALSE 0x00

/* External variables --------------------------------------------------------*/
GLOBAL const K_ModuleItem_Typedef ModuleMath;

/* Peripherals HAL handles */
GLOBAL CORDIC_HandleTypeDef  CordicHandle;
GLOBAL DMA_HandleTypeDef     DmaCordicWriteHandle;
GLOBAL DMA_HandleTypeDef     DmaCordicReadHandle;
GLOBAL FMAC_HandleTypeDef    FmacHandle;
GLOBAL DMA_HandleTypeDef     DmaFmacInHandle;
GLOBAL DMA_HandleTypeDef     DmaFmacOutHandle;
GLOBAL DMA_HandleTypeDef     DmaFmacPreloadHandle;

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
