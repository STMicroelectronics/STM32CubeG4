/**
  @page TIM_OCToggle TIM_OCToggle example
  
  @verbatim
  ******************************************************************************
  * @file    TIM/TIM_OCToggle/readme.txt 
  * @author  MCD Application Team
  * @brief   This example shows how to configure the Timer to generate four different 
  *          signals with four different frequencies.
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

Configuration of the TIM peripheral to generate four different 
signals at four different frequencies.

  At the beginning of the main program the HAL_Init() function is called to reset 
  all the peripherals, initialize the Flash interface and the systick.
  The SystemClock_Config() function is used to configure the system clock for STM32G474RETx Devices :
  The CPU at 170 MHz 
  The TIM1 frequency is set to SystemCoreClock, and the objective is
  to get TIM1 counter clock at 17 MHz so the Prescaler is computed as following:
     - Prescaler = (TIM1CLK /TIM1 counter clock) - 1
   
  SystemCoreClock is set to 170 MHz for STM32G4xx Devices.

  The TIM1 CCR1 register value is equal to 4000:
  CC1 update rate = TIM1 counter clock / CCR1_Val = 4250 Hz,
  so the TIM1 Channel 1 generates a periodic signal with a frequency equal to 2125 Hz.

  The TIM1 CCR2 register value is equal to 2000:
  CC2 update rate = TIM1 counter clock / CCR2_Val = 8500 Hz,
  so the TIM1 channel 2 generates a periodic signal with a frequency equal to 4250 Hz.

  The TIM1 CCR3 register value is equal to 1000:
  CC3 update rate = TIM1 counter clock / CCR3_Val = 17000  Hz,
  so the TIM1 channel 3 generates a periodic signal with a frequency equal to 8500 Hz.

  The TIM1 CCR4 register value is equal to 500:
  CC4 update rate = TIM1 counter clock / CCR4_Val =  34000 Hz,
  so the TIM1 channel 4 generates a periodic signal with a frequency equal to 17000 Hz.

@note PWM signal frequency values mentioned above are theoretical (obtained when the system clock frequency
      is exactly 170 MHz). Since the generated system clock frequency may vary from one board to another observed
      PWM signal frequency might be slightly different.
	  
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note This example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Timer, Output, signals, Output compare toggle, PWM, Oscilloscope

@par Directory contents

  - TIM/TIM_OCToggle/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_OCToggle/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_OCToggle/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - TIM/TIM_OCToggle/Inc/main.h                  Header for main.c module  
  - TIM/TIM_OCToggle/Src/stm32g4xx_it.c          Interrupt handlers
  - TIM/TIM_OCToggle/Src/main.c                  Main program
  - TIM/TIM_OCToggle/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - TIM/TIM_OCToggle/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474RETx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-G474RE 
    board and can be easily tailored to any other supported device 
    and development board.      

  - NUCLEO-G474RE Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
      - PA8: (TIM1_CH1) (pin 23 in CN10 connector)
      - PA9: (TIM1_CH2) (pin 21 in CN10 connector)
      - PA10: (TIM1_CH3) (pin 33 in CN10 connector)
      - PA11: (TIM1_CH4) (pin 14 in CN10 connector)

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
