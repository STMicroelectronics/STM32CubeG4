/**
  @page HRTIM_Basic_PWMMaster HRTIM example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/HRTIM/HRTIM_Basic_PWMMaster/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the HRTIM_Basic_PWMMaster example.
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
  This example describes how to generate basic PWM waveforms with HRTIM timers 
  other than the timing unit itself, as per HRTIM Cookbook basic examples (refer to AN4539 Application note).

  The PWM are generated on the TD1 and TD2 outputs (resp. PB14 and PB15) with a frequency of 100kHz.

  Green LED: blinks during normal operation
  Green LED: ON when the ERROR handler is entered

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

HRTIM, Timers, PWM, Waveforms, PWM master, Frequency.

@par Directory contents 

  - HRTIM/HRTIM_Basic_PWMMaster/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - HRTIM/HRTIM_Basic_PWMMaster/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - HRTIM/HRTIM_Basic_PWMMaster/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - HRTIM/HRTIM_Basic_PWMMaster/Inc/main.h                  Header for main.c module  
  - HRTIM/HRTIM_Basic_PWMMaster/Src/stm32g4xx_it.c          IT interrupt handlers
  - HRTIM/HRTIM_Basic_PWMMaster/Src/main.c                  Main program
  - HRTIM/HRTIM_Basic_PWMMaster/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - HRTIM/HRTIM_Basic_PWMMaster/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment 

  - This example runs on STM32G474RETx devices.
  
  - This example has been tested with two NUCLEO-G474RE RevC boards embedding
    a STM32G474RET6 device and can be easily tailored to any other supported device 
    and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 */
