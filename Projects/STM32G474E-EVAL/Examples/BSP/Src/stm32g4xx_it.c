/**
  ******************************************************************************
  * @file    BSP/Src/stm32g4xx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include "stm32g4xx_it.h"

/* USER CODE BEGIN 0 */
extern SAI_HandleTypeDef                haudio_out_sai;

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
#if (USE_BSP_IO_CLASS == 1)
extern __IO uint32_t IddOnGoing;
#endif
/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
/**
  * @brief  This function handles DMA1 Channel 1 interrupt request.
  * @param  None
  * @retval None
  */
void SRAM_DMAx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hsram.hdma);
}

/* this IRQ is for MFX */
void EXTI0_IRQHandler(void)
{
#if (USE_BSP_IO_CLASS == 1)
  if (IddOnGoing ==1)
  {
    BSP_IDD_IRQHandler();
  }
  else
  {
    BSP_JOY_IRQHandler();
  }
#endif
}

void EXTI1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

/**
  * @brief  This function handles EXT 10 to 15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  BSP_PB_IRQHandler(BUTTON_USER);
}

/**
  * @brief  This function handles SAI DMA interrupt request.
  * @param  None
  * @retval None
  */
void AUDIO_SAI1_DMAx_IRQHandler(void)
{
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/**
  * @brief  This function handles SAI Tx DMA interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Channel1_IRQHandler(void)
{
    BSP_AUDIO_OUT_DMA_IRQHandler(0);
}

/**
  * @brief  This function handles SAI Rx DMA interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Channel2_IRQHandler(void)
{
    BSP_AUDIO_IN_DMA_IRQHandler(0, AUDIO_IN_DEVICE_ANALOG_MIC);
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/* USER CODE END 1 */

