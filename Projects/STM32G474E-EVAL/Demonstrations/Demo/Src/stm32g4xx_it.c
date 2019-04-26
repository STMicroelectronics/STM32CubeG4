/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @author  MCD Application Team
  * @brief   This file includes the interrupt handlers for the application.
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


/** @addtogroup CORE
  * @{
  */

/** @defgroup
  * @brief
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* RTC Handle variable */
extern RTC_HandleTypeDef RtcHandle;

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/
/**
  * @brief  NMI_Handler
  *         This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  HardFault_Handler
  *         This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  MemManage_Handler
  *         This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  BusFault_Handler
  *         This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  UsageFault_Handler
  *         This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  DebugMon_Handler
  *         This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/*                 STM32G4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32gxxx.s).                                                */
/******************************************************************************/
void SysTick_Handler (void)
{
  HAL_IncTick();
  if (UcpdDemoRunning == 1)
  {
    USBPD_DPM_TimerCounter();
  }
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
