/**
  @page TIM_DMA TIM DMA example
  
  @verbatim
  ******************************************************************************
  * @file    TIM/TIM_DMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the TIM DMA example.
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

Use of the DMA with TIMER Update request 
to transfer data from memory to TIMER Capture Compare Register 3 (TIMx_CCR3).

  The following configuration values are used in this example:

    - TIM3CLK = SystemCoreClock
    - Counter repetition = 3 
    - Prescaler = 0 
    - TIM3 counter clock = SystemCoreClock
    - SystemCoreClock is set to 150 MHz for STM32G4xx

  The objective is to configure TIM3 channel 3 to generate complementary PWM 
  (Pulse Width Modulation) signal with a frequency equal to 17.57 KHz, and a variable 
  duty cycle that is changed by the DMA after a specific number of Update DMA request.

  The number of this repetitive requests is defined by the TIM3 Repetition counter,
  each 4 Update Requests, the TIM3 Channel 3 Duty Cycle changes to the next new 
  value defined by the aCCValue_Buffer.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32G431KBTx Devices :
The CPU at 150 MHz  

  The PWM waveform can be displayed using an oscilloscope.
 
  In nominal mode (except at start) , it should looks like this :
    
       .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   
        ___________     _______         ___             ___________    __
      _|           |___|       |_______|   |___________|           |__|             
       <----57us -----><----57us -----><----57us -----><----57us ----->

	   
@note PWM signal frequency value mentioned above is theoretical (obtained when the system clock frequency 
      is exactly 150 MHz). Since the generated system clock frequency may vary from one board to another observed
      PWM signal frequency might be slightly different.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)

@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

	  
@par Keywords

Timer, DMA, PWM, Frequency, Duty Cycle, Waveform, Oscilloscope, Output, Signal

@par Directory contents

  - TIM/TIM_DMA/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_DMA/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_DMA/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - TIM/TIM_DMA/Inc/main.h                  Header for main.c module  
  - TIM/TIM_DMA/Src/stm32g4xx_it.c          Interrupt handlers
  - TIM/TIM_DMA/Src/main.c                  Main program
  - TIM/TIM_DMA/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - TIM/TIM_DMA/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G431KBTx devices.
  - In this example, the clock is set to 150 MHz.
    
  - This example has been tested with STMicroelectronics NUCLEO-G431KB 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-G431KB Set-up
    - Connect the TIM3 pin to an oscilloscope to monitor the different waveforms: 
    - TIM3 CH3 (PB.0 (pin 6 in CN4 connector))

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 */
