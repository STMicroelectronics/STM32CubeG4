/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32g4xx_it.c
  * @author       STMicroelectronics
  * @version      V1.0.0
  * @date         06-Jan-2022
  * @brief        Interrupt Service Routines
  * @details      This file contains the source code of ISRs
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  *
 * @history Date       Version Person  Change
 * @history ---------- ------- ------- ----------------------------------------
  * @history 2022-01-06 1.0     NSa    First version of the file
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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
uint16_t zcd_re_time = 30000; 
uint16_t oc_re_time = 14000; 

uint32_t irq_flags = 0;

// The following is a status variable to chose which blanking window we want 
// to represent on PF1:
//      - 0: display ZCD blanking window
//      - 1: display OC blanking window
char blanking_display_status = 0;  

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern HRTIM_HandleTypeDef hhrtim1;
extern UART_HandleTypeDef huart3;
extern PCD_HandleTypeDef hpcd_USB_FS;
/* USER CODE BEGIN EV */

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
  while (1)
  {
  }
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

  /* JOYSTICK UP increases ZCD rising edge time w.r.t. PWM set */
  if (HAL_GPIO_ReadPin(JOYSTICK_UP_GPIO_Port, JOYSTICK_UP_Pin) == GPIO_PIN_RESET)
  {
     zcd_re_time += EVENT_DELTA_WHEN_BUTTON_PRESSED;
     if (zcd_re_time >= MAX_DUTY_CYCLE)
      zcd_re_time = MAX_DUTY_CYCLE;
  }

  /* JOYSTICK DOWN decreases ZCD rising edge time w.r.t. PWM set */
  if (HAL_GPIO_ReadPin(JOYSTICK_DOWN_GPIO_Port, JOYSTICK_DOWN_Pin) == GPIO_PIN_RESET)
  {
    zcd_re_time -= EVENT_DELTA_WHEN_BUTTON_PRESSED;
     if (zcd_re_time <= oc_re_time + OC_PULSE_WIDTH)
         zcd_re_time += EVENT_DELTA_WHEN_BUTTON_PRESSED;
  }      

  /* JOYSTICK RIGHT increases 2nd OC rising edge time w.r.t. PWM set */
  if (HAL_GPIO_ReadPin(JOYSTICK_RIGHT_GPIO_Port, JOYSTICK_RIGHT_Pin) == GPIO_PIN_RESET)
  {
   oc_re_time += EVENT_DELTA_WHEN_BUTTON_PRESSED;
     if (oc_re_time >= zcd_re_time - OC_PULSE_WIDTH)
       oc_re_time -= EVENT_DELTA_WHEN_BUTTON_PRESSED;
  }

  /* JOYSTICK LEFT decreases 2nd OC rising edge time w.r.t. PWM set */
  if (HAL_GPIO_ReadPin(JOYSTICK_LEFT_GPIO_Port, JOYSTICK_LEFT_Pin) == GPIO_PIN_RESET)
  {
   oc_re_time -= EVENT_DELTA_WHEN_BUTTON_PRESSED;
   if(oc_re_time < 10)
     oc_re_time += EVENT_DELTA_WHEN_BUTTON_PRESSED;

  }

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

/**
  * @brief This function handles ADC1 and ADC2 global interrupt.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
  * @brief This function handles USB high priority interrupt remap.
  */
void USB_HP_IRQHandler(void)
{
  /* USER CODE BEGIN USB_HP_IRQn 0 */

  /* USER CODE END USB_HP_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_HP_IRQn 1 */

  /* USER CODE END USB_HP_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt / USART3 wake-up interrupt through EXTI line 28.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(JOYSTICK_UP_Pin);
  HAL_GPIO_EXTI_IRQHandler(JOYSTICK_SEL_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles HRTIM timer F global interrupt.
  */
void HRTIM1_TIMF_IRQHandler(void)
{
  /* USER CODE BEGIN HRTIM1_TIMF_IRQn 0 */
  irq_flags = (&hhrtim1)->Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_F].TIMxISR;
  (&hhrtim1)->Instance->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_F].TIMxICR = irq_flags;

  if((irq_flags & HRTIM_TIM_IT_RST) == HRTIM_TIM_IT_RST){
    // If the source event is timer reset (Toff Max or ZCD)
      
      // Update the ZCD pulse parameters
      __HAL_HRTIM_SETCOMPARE(&hhrtim1, 
                             HRTIM_TIMERINDEX_TIMER_E,
                             HRTIM_COMPAREUNIT_3,
                             zcd_re_time);

      __HAL_HRTIM_SETCOMPARE(&hhrtim1,
                             HRTIM_TIMERINDEX_TIMER_E,
                             HRTIM_COMPAREUNIT_4,
                             zcd_re_time + ZCD_PULSE_WIDTH);

      // Update the OC pulse parameters
      __HAL_HRTIM_SETCOMPARE(&hhrtim1, 
                             HRTIM_TIMERINDEX_TIMER_F,
                             HRTIM_COMPAREUNIT_1,
                             oc_re_time);

      __HAL_HRTIM_SETCOMPARE(&hhrtim1,
                             HRTIM_TIMERINDEX_TIMER_F,
                             HRTIM_COMPAREUNIT_2,
                             oc_re_time + OC_PULSE_WIDTH);

  }

  /* USER CODE END HRTIM1_TIMF_IRQn 0 */
  /* USER CODE BEGIN HRTIM1_TIMF_IRQn 1 */

  /* USER CODE END HRTIM1_TIMF_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
