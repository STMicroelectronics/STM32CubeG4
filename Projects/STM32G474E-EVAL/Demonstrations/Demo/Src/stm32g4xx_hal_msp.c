/**
  ******************************************************************************
  * @file    stm32g4xx_hal_msp.c
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

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief ADC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hadc  ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC Periph clock enable */
  __HAL_RCC_ADC12_CLK_ENABLE();
  /* Enable GPIO clock ****************************************/
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* ADC Channel GPIO pin configuration */
  GPIO_InitStruct.Pin  = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  ADC MSP De-Initialization
  * @param  hadc  ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  /* Disable the ADC Periheral clock */
  __HAL_RCC_ADC12_CLK_DISABLE();

  /* Force the ADC Periheral Clock Reset */
  __HAL_RCC_ADC12_FORCE_RESET();
}

/**
  * @brief  RTC MSP Initialization
  * @param  hrtc  RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct ={0};

  /* Configure LSE as RTC clock source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    while(1);
  }

  /* Enable RTC peripheral Clocks */
  __HAL_RCC_RTC_ENABLE();

  /* Configure NVIC */
  HAL_NVIC_SetPriority(RTC_TAMP_LSECSS_IRQn, 0x03u, 0x00u);
  HAL_NVIC_EnableIRQ(RTC_TAMP_LSECSS_IRQn);
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x03u, 0x00u);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}


/**
  * @brief  RTC MSP De-Initialization
  * @param  hrtc RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /* Disable Irq */
  HAL_NVIC_DisableIRQ(RTC_TAMP_LSECSS_IRQn);
}


