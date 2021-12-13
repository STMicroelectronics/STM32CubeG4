/**
  ******************************************************************************
  * @file    app_whiteled.h
  * @author  MCD Application Team
  * @brief   White led application interface
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


