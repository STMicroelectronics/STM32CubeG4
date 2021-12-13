/**
  @page HRTIM_Basic_PWM_Master HRTIM example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/HRTIM/HRTIM_Basic_PWM_Master/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the HRTIM_Basic_PWM_Master example.
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
  This example is based on the STM32G4xx HRTIM LL API. 
  The peripheral uses LL unitary service functions.

  The PWM are generated on the TD1 and TD2 outputs (resp. PB14 and PB15) with a frequency of 100kHz.

  Green LED: blinks during normal operation
  Green LED: ON when the ERROR handler is entered

@par Keywords

HRTIM, Timers, PWM, waveforms, PWM master, frequency.

@par Directory contents 

  - HRTIM/HRTIM_Basic_PWM_Master/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - HRTIM/HRTIM_Basic_PWM_Master/Inc/main.h                  Header for main.c module  
  - HRTIM/HRTIM_Basic_PWM_Master/Src/stm32g4xx_it.c          IT interrupt handlers
  - HRTIM/HRTIM_Basic_PWM_Master/Src/main.c                  Main program
  - HRTIM/HRTIM_Basic_PWM_Master/Src/system_stm32g4xx.c      STM32G4xx system source file

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
