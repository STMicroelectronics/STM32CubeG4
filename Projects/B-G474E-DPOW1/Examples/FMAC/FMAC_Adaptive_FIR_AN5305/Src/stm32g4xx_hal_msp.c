/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FMAC/FMAC_Adaptive_FIR_AN5305/Src/stm32g4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
extern DMA_HandleTypeDef hdma_fmac_preload;

extern DMA_HandleTypeDef hdma_fmac_write;

extern DMA_HandleTypeDef hdma_fmac_read;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  HAL_PWREx_DisableUCPDDeadBattery();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief FMAC MSP Initialization
* This function configures the hardware resources used in this example
* @param hfmac: FMAC handle pointer
* @retval None
*/
void HAL_FMAC_MspInit(FMAC_HandleTypeDef* hfmac)
{
  if(hfmac->Instance==FMAC)
  {
  /* USER CODE BEGIN FMAC_MspInit 0 */

  /* USER CODE END FMAC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_FMAC_CLK_ENABLE();

    /* FMAC DMA Init */
    /* FMAC_PRELOAD Init */
    hdma_fmac_preload.Instance = DMA1_Channel1;
    hdma_fmac_preload.Init.Request = DMA_REQUEST_MEM2MEM;
    hdma_fmac_preload.Init.Direction = DMA_MEMORY_TO_MEMORY;
    hdma_fmac_preload.Init.PeriphInc = DMA_PINC_ENABLE;
    hdma_fmac_preload.Init.MemInc = DMA_MINC_DISABLE;
    hdma_fmac_preload.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_fmac_preload.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_fmac_preload.Init.Mode = DMA_NORMAL;
    hdma_fmac_preload.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_fmac_preload) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hfmac,hdmaPreload,hdma_fmac_preload);

    /* FMAC_WRITE Init */
    hdma_fmac_write.Instance = DMA1_Channel2;
    hdma_fmac_write.Init.Request = DMA_REQUEST_FMAC_WRITE;
    hdma_fmac_write.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_fmac_write.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_fmac_write.Init.MemInc = DMA_MINC_ENABLE;
    hdma_fmac_write.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_fmac_write.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_fmac_write.Init.Mode = DMA_NORMAL;
    hdma_fmac_write.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_fmac_write) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hfmac,hdmaIn,hdma_fmac_write);

    /* FMAC_READ Init */
    hdma_fmac_read.Instance = DMA1_Channel3;
    hdma_fmac_read.Init.Request = DMA_REQUEST_FMAC_READ;
    hdma_fmac_read.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_fmac_read.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_fmac_read.Init.MemInc = DMA_MINC_ENABLE;
    hdma_fmac_read.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_fmac_read.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_fmac_read.Init.Mode = DMA_NORMAL;
    hdma_fmac_read.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_fmac_read) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(hfmac,hdmaOut,hdma_fmac_read);

    /* FMAC interrupt Init */
    HAL_NVIC_SetPriority(FMAC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(FMAC_IRQn);
  /* USER CODE BEGIN FMAC_MspInit 1 */

  /* USER CODE END FMAC_MspInit 1 */
  }

}

/**
* @brief FMAC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hfmac: FMAC handle pointer
* @retval None
*/
void HAL_FMAC_MspDeInit(FMAC_HandleTypeDef* hfmac)
{
  if(hfmac->Instance==FMAC)
  {
  /* USER CODE BEGIN FMAC_MspDeInit 0 */

  /* USER CODE END FMAC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FMAC_CLK_DISABLE();

    /* FMAC DMA DeInit */
    HAL_DMA_DeInit(hfmac->hdmaPreload);
    HAL_DMA_DeInit(hfmac->hdmaIn);
    HAL_DMA_DeInit(hfmac->hdmaOut);

    /* FMAC interrupt DeInit */
    HAL_NVIC_DisableIRQ(FMAC_IRQn);
  /* USER CODE BEGIN FMAC_MspDeInit 1 */

  /* USER CODE END FMAC_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
