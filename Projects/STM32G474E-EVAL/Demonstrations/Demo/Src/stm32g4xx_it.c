/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_sd.h"
#include "stm32g474e_eval_audio.h"
#include "app_audio.h"
#include "app_calendar.h"
#include "app_math.h"
#include "app_whiteled.h"
#include "app_ucpd.h"
#include "k_config.h"
#include "k_module.h"
#include "k_storage.h"
#include "k_menu.h"
#include "main.h"
#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_hw_if.h"
#if defined(_TRACE) || defined(_GUI_INTERFACE)
#include "tracer_emb.h"
#endif /*_TRACE || _GUI_INTERFACE */
#if defined(_GUI_INTERFACE)
#include "gui_api.h"
#endif /* _GUI_INTERFACE */
/* USER CODE BEGIN Includes */


/** @addtogroup CORE
  * @{
  */

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* RTC Handle variable */
extern RTC_HandleTypeDef RtcHandle;

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}
/******************************************************************************/
/*                 STM32G4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32gxxx.s).                                                */
/******************************************************************************/
/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler (void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  if (UcpdDemoRunning == 1)
  {
    USBPD_DPM_TimerCounter();
#if defined(_GUI_INTERFACE)
    GUI_TimerCounter();
#endif /* _GUI_INTERFACE */
  }
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  /* USER CODE END SysTick_IRQn 1 */
}

/**
  * @brief  This function handles RTC interrupt request.
  * @param  None
  * @retval None
  */
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

/**
  * @brief  This function handles External lines 10_15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  BSP_PB_IRQHandler(BUTTON_USER);
}

/**
  * @brief  This function handles External lines 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  BSP_JOY_IRQHandler();
}

/**
  * @brief  This function handles UCPD1 interrupts.
  * @param  None
  * @retval None
  */
void UCPD1_IRQHandler(void)
{
  USBPD_PORT0_IRQHandler();
}

/**
  * @brief  DMA1 channel 3 interrupt handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&DmaCordicWriteHandle);
}

/**
  * @brief  DMA1 channel 4 interrupt handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&DmaCordicReadHandle);
}

/**
  * @brief  DMA1 channel 5 interrupt handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&DmaFmacInHandle);
}

/**
  * @brief  DMA1 channel 6 interrupt handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel6_IRQHandler(void)
{
  if (UcpdDemoRunning == 1)
  {
#if defined(_TRACE) || defined(_GUI_INTERFACE)
    TRACER_EMB_IRQHandlerDMA();
#endif /*_TRACE || _GUI_INTERFACE */
  }
  else
  {
    HAL_DMA_IRQHandler(&DmaFmacOutHandle);
  }
}

/**
  * @brief  This function handles USART interrupts (used for GUI).
  * @param  None
  * @retval None
  */
void TRACER_EMB_USART_IRQHANDLER(void)
{
  if (UcpdDemoRunning == 1)
  {
    TRACER_EMB_IRQHandlerUSART();
  }
}

/**
  * @brief  DMA1 channel 7 interrupt handler.
  * @param  None
  * @retval None
  */
void DMA1_Channel7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&DmaFmacPreloadHandle);
}

/**
  * @brief  This function handles HRTIM1 Master interrupt request.
  * @param  None
  * @retval None
  */
void HRTIM1_Master_IRQHandler(void)
{
  HAL_HRTIM_IRQHandler(&HrtimHandle, HRTIM_TIMERINDEX_MASTER);
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
  BSP_AUDIO_IN_DMA_IRQHandler(0,AUDIO_IN_DEVICE_ANALOG_MIC);
}

/**
  * @}
  */

/**
  * @}
  */



