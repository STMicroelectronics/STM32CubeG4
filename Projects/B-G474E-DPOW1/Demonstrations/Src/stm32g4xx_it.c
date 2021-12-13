/**
  ******************************************************************************
  * @file    Demonstrations/Src/stm32g4xx_it.c
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
#include "BuckBoost.h"
#ifdef DSMPS_DEBUG
#include "stm32g4xx_ll_gpio.h"
#endif /* DSMPS_DEBUG */


/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern FMAC_HandleTypeDef hfmac;
extern HRTIM_HandleTypeDef hhrtim1;


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
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
  * @brief  This function handles Memory Manage exception.
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
  * @brief  This function handles Bus Fault exception.
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
  * @brief  This function handles Usage Fault exception.
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
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32G4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32g4xxxx.s).                                             */
/******************************************************************************/

/**
  * @brief  This function handles External lines 2 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  /* Call exti IRQ handler for RIGHT direction */
  HAL_EXTI_IRQHandler(&hjoy_exti[3]);
}

/**
  * @brief  This function handles External lines 4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
  /* Call exti IRQ handler for LEFT direction */
  HAL_EXTI_IRQHandler(&hjoy_exti[2]);
}


/**
  * @brief  This function handles External lines 5_9 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  /* Call exti IRQ handler for DOWN direction */
  HAL_EXTI_IRQHandler(&hjoy_exti[1]);
}

/**
  * @brief  This function handles External lines 10_15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  /* Call exti IRQ handler for SEL */
  HAL_EXTI_IRQHandler(&hjoy_exti[0]);
  /* Call exti IRQ handler for UP direction */
  HAL_EXTI_IRQHandler(&hjoy_exti[4]);
}


/**
* @brief  This function handles HRTIM TIMF interrupts requests.
* @param  None
* @retval None
*/
void HRTIM1_TIMF_IRQHandler(void)
{
  HAL_HRTIM_IRQHandler(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F);
}

/**
  * @brief This function handles FMAC interrupt.
  */
void FMAC_IRQHandler(void)
{
#ifdef DSMPS_DEBUG
  /* Set GPO1 to high for timing purposes */
  LL_GPIO_SetOutputPin(GPO1_GPIO_Port, GPO1_Pin);
#endif /* DSMPS_DEBUG */

  /* Update PWM */
  uint32_t m_Out = READ_REG(hfmac.Instance->RDATA);
  m_Out = (m_Out > 0x00007FFF ? 0 : m_Out);
  switch (StateMachine)
  {
  case STATE_BUCK:
    __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_C, HRTIM_COMPAREUNIT_1, m_Out);
    break;

  case STATE_BUCKBOOST:
    __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_1, m_Out);
    break;

  default :
    Error_Handler();
    break;
  }

#ifdef DSMPS_DEBUG
  /* Set GPO1 to low for timing purposes */
  LL_GPIO_ResetOutputPin(GPO1_GPIO_Port, GPO1_Pin);
#endif /* DSMPS_DEBUG */
}

/**
  * @}
  */

/**
  * @}
  */


