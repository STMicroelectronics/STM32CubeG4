/**
  @page HRTIM_CBC_Deadtime HRTIM example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/HRTIM/HRTIM_CBC_Deadtime/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of HRTIM_CBC_Deadtime example.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description 
  This example describes how to implement a cycle-by-cycle (CBC) current control 
  with complementary signals and dead time insertion.
  An overcurrent condition will turn-off the two complementary outputs, without 
  re-arming the complementary output.
  Refer to the  HRTIM Cookbook for further details.

  Two complementary PWM waveforms are generated on the TD1 and TD2 outputs 
  (PB14 and PB15).
  The over-current digital signal must be connected to both EEV5 and EEV8 external event 
  lines on PB8 and PB9 inputs.

  Green LED: blinks during normal operation on Nucleo board, not active on Discovery board.
  Green LED: ON when the ERROR handler is entered

  The PWM frequency and dead time duration can be 
  entered respectively in kHz and in ns, using the following User Constants:
  - TIMER_PWM_FREQ
  - DEADTIME_IN_NS

@par Keywords

HRTIM, Timers, PWM, Waveforms, Frequency, Cycle by cycle, CBC, External event, Dead time.


@par Directory contents 

  - HRTIM/HRTIM_CBC_Deadtime/Inc/main.h                  Header for main.c module  
  - HRTIM/HRTIM_CBC_Deadtime/Src/main.c                  Main program
  - HRTIM/HRTIM_CBC_Deadtime/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - HRTIM/HRTIM_CBC_Deadtime/Src/stm32g4xx_it.c          IT interrupt handlers
  - HRTIM/HRTIM_CBC_Deadtime/Src/system_stm32g4xx.c      STM32G4xx system source file

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

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
