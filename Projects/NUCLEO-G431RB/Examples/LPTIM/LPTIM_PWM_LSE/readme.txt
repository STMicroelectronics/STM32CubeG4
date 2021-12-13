/**
  @page LPTIM_PWM_LSE LPTIM PWM Low Power example with LSE clock source
  
  @verbatim
  ******************************************************************************
  * @file    LPTIM/LPTIM_PWM_LSE/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the LPTIM PWM LSE example
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
  @endverbatim

@par Example Description 

How to configure and use, through the HAL LPTIM API, the LPTIM peripheral using LSE 
as counter clock, to generate a PWM signal, in a low-power mode.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32G431RBTx Devices :
The CPU at 170 MHz 

In this example, the LPTIM instance used is LPTIM1 and the low power mode is Stop mode.


The counter clock is LSE (32.768 KHz), Autoreload equal to 99 so the output
frequency (FrequencyOutput) will be equal to 327.680.

  FrequencyOutput = Counter Clock Frequency / (Autoreload + 1)
                  = 32768 / 100
                  = 327.680 Hz

Pulse value equal to 49 and the duty cycle (DutyCycle) is computed as follow:

  DutyCycle = 1 - ((PulseValue + 1)/ (Autoreload + 1))
  DutyCycle = 50%

To minimize the power consumption, after starting generating the PWM signal,
the MCU enters in Stop mode. Note that GPIOs are configured in Low Speed to
enhance the consumption. 

User push-button pin (PC.13)is configured as input with external interrupt (External line 13),
falling edge. When User push-button is pressed, wakeup event is generated and PWM signal
generation is stopped.

@note This example can not be used in DEBUG mode, this is due to the fact
      that the Cortex-M4 core is no longer clocked during low power mode
      so debugging features are disabled.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.


@par Keywords

Timer, Low Power, PWM, Stop mode, Interrupt, LSE, Output, Duty Cycle

@par Directory contents

  - LPTIM/LPTIM_PWM_LSE/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - LPTIM/LPTIM_PWM_LSE/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - LPTIM/LPTIM_PWM_LSE/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_PWM_LSE/Inc/main.h                  Header for main.c module  
  - LPTIM/LPTIM_PWM_LSE/Src/stm32g4xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_PWM_LSE/Src/main.c                  Main program
  - LPTIM/LPTIM_PWM_LSE/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - LPTIM/LPTIM_PWM_LSE/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-G431RB
    board and can be easily tailored to any other supported device
    and development board.      

  - Connect PB2 (pin 22 in CN10 connector) to an oscilloscope to monitor the LPTIM1 output waveform.
  
  - LSE oscillator must be mounted on the board, else this example does not work.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 

 */
