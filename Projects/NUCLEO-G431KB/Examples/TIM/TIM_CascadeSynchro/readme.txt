/**
  @page TIM_CascadeSynchro Timers Synchronization example
  
  @verbatim
  ******************************************************************************
  * @file    TIM/TIM_CascadeSynchro/readme.txt 
  * @author  MCD Application Team
  * @brief   How to command 2 Timers as slaves (TIM3 & TIM4) using a Timer
  *          as master (TIM2)

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

This example shows how to synchronize TIM2 and Timers (TIM3 and TIM4) in cascade mode.

Timers synchronisation in cascade mode:
   ___________                     ___________              ___________                 
  |   MASTER  |TRGO_Update   ITR1 |  SLAVE 1  |       ITR2 |  SLAVE 2  |                 
  |    TIM2   |-------------------|    TIM3   |------------|    TIM4   |               
  |___________|                   |___________|            |___________| 
                                 

1/ TIM2 is configured as Master Timer:
   - PWM Mode is used
   - The TIM2 Update event is used as Trigger Output 
 
2)TIM3 is slave for TIM2 and Master for TIM4,
  - PWM Mode is used
  - The ITR1(TIM2) is used as input trigger 
  - Gated mode is used, so start and stop of slave counter
    are controlled by the Master trigger output signal(TIM2 update event).
  - The TIM3 Update event is used as Trigger Output.

3)TIM4 is slave for TIM3,
  - PWM Mode is used
  - The ITR2(TIM3) is used as input trigger
  - Gated mode is used, so start and stop of slave counter  are controlled by the
    Master trigger output signal(TIM3 update event).

The TIM2 counter clock is 170 MHz.

  The Master Timer TIM2 is running at:
  TIM2 frequency = TIM2 counter clock / (TIM2_Period + 1) = 665.5 KHz and
  a the duty cycle equal to: TIM2_CCR1/(TIM2_ARR + 1) = 25%

  The TIM3 is running at: 
  (TIM2 frequency)/ (TIM3 period +1) = 166.6 KHz and
  a duty cycle equal to TIM3_CCR1/(TIM3_ARR + 1) = 25%

  The TIM4 is running at:
  (TIM3 frequency)/ (TIM4 period +1) = 41.65 KHz and
  a duty cycle equal to TIM4_CCR1/(TIM4_ARR + 1) = 25%


The PWM waveform can be displayed using an oscilloscope.

@par Keywords

Timer, PWM, Cascade Synchronization, Master, Slave, Duty Cycle, Waveform, Oscilloscope, Output, Signal

@par Directory contents

  - TIM/TIM_CascadeSynchro/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_CascadeSynchro/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_CascadeSynchro/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - TIM/TIM_CascadeSynchro/Inc/main.h                  Header for main.c module  
  - TIM/TIM_CascadeSynchro/Src/stm32g4xx_it.c          Interrupt handlers
  - TIM/TIM_CascadeSynchro/Src/main.c                  Main program
  - TIM/TIM_CascadeSynchro/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - TIM/TIM_CascadeSynchro/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G431KBTx devices.
    
  - This example has been tested with NUCLEO-G431KB board and can be
    easily tailored to any other supported device and development board.      

  - NUCLEO-G431KB Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
      - TIM2 CH1 (PA5 (pin 11 in CN10 connector))
      - TIM3 CH1 (PA6 (pin 13 in CN10 connector))
      - TIM4 CH1 (PA11 (pin 14 in CN10 connector))

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5) 

 */
