/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "b_g474e_dpow1.h"

/* USER CODE END Includes */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern uint32_t DemoState;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

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
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
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
  * @brief This function handles HRTIM timer C global interrupt.
  */
void HRTIM1_TIMC_IRQHandler(void)
{
  uint32_t CurrentDuty;

  /* Clear ISR flag */
  LL_HRTIM_ClearFlag_REP(HRTIM1, LL_HRTIM_TIMER_C);

  switch(DemoState)
  {
  case BUCK:
    /* Get current duty cycle value */
    CurrentDuty = LL_HRTIM_TIM_GetCompare1(HRTIM1, LL_HRTIM_TIMER_C);
    /* Increase the compare value up to 60% duty cycle and re-start from 33% */
    if (CurrentDuty > (6 * BUCK_PWM_PERIOD)/10)
    {
      LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_C, BUCK_PWM_PERIOD/3);
      LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_C, BUCK_PWM_PERIOD/6);
    }
    else  /* Re-start ramp from 33% duty cycle */
    {
      /* Update duty cycle (CMP1) and ADC triggering point (CMP2)*/
      CurrentDuty++;
      LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_C, CurrentDuty);
      LL_HRTIM_TIM_SetCompare2(HRTIM1, LL_HRTIM_TIMER_C, CurrentDuty/2);
    }
    /* BOOST side PMOS turned ON permanently */
    LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_D, BUCK_PWM_PERIOD+1);
    break;

  case BOOST:
    /* BUCK side PMOS turned ON permanently */
    LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_C, BUCK_PWM_PERIOD);
    
    /* Get current boost duty cycle value */
    CurrentDuty = LL_HRTIM_TIM_GetCompare1(HRTIM1,  LL_HRTIM_TIMER_D);
    // Slowly increase boost factor to limit inrush current (Boost factor is 1/CurrentDuty)
    CurrentDuty -= 0x400;
    /* Increase boost factor up to 50% */
    if (CurrentDuty >= (BUCK_PWM_PERIOD/2) + 1 )
    {
      LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_D, CurrentDuty);
    }
    break;

  case FAULT:
  case DE_ENERGIZE:
    /* BUCK side NMOS turned ON permanently */
    LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_C, BUCK_PWM_PERIOD+1);
    /* BOOST side NMOS turned ON permanently */
    LL_HRTIM_TIM_SetCompare1(HRTIM1, LL_HRTIM_TIMER_D, BUCK_PWM_PERIOD);
    break;
    
  default:
    /* Something wrong happened, shut-down all switches */
    LL_HRTIM_DisableOutput(HRTIM1, 
                              LL_HRTIM_OUTPUT_TC1
                            | LL_HRTIM_OUTPUT_TC2
                            | LL_HRTIM_OUTPUT_TD1
                            | LL_HRTIM_OUTPUT_TD2);
    break;
  }
}
/* USER CODE END 1 */
