/**
  @page HRTIM_Dual_Buck HRTIM example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_MIX/HRTIM/HRTIM_Dual_Buck/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the HRTIM_Dual_Buck example.
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
  This example shows how to configure the HRTIM to have 2 buck converters 
  controlled by a single timer unit.
  This example uses the STM32G4xx HRTIM HAL and LL API.

  This is done using a single timer unit (Timer D), with the TD1 and TD2 outputs
  (resp. PB14 and PB15).
  The duty cycle on TD1 is continuously varied in an interrupt service routine 
  generated on repetition event, to mimic a real converter management. 
  
  The FAULT2 input is enabled on PA15 (active low) to demonstrate PWM shut down 
  (low level sensitive).
  When the fault is triggered (PA15 input connected to GND), TD1 signal only is
  stopped. The system can be re-armed by pressing the user button.

  LEDs are indicating the following:
  Green LED: blinks during normal operation
  Red LED: blinks when FAULT is triggered
  Orange LED: indicates the occurrence and duration of the PWM refresh ISR.

  The ADC is configured to have conversions triggered in the middle of the
  ON time of each converter (non-overlapping PWM signals), on PA1 input.
  The TD1 output (PB14) can be connected to the RC filter input (PC7) for monitoring
  the low-pass filtered voltage on TP4 test point.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

HRTIM, Timers, Buck converter, PWM , Low level, ADC, Duty cycle. 

@par Directory contents 

  - HRTIM/HRTIM_Dual_Buck/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - HRTIM/HRTIM_Dual_Buck/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - HRTIM/HRTIM_Dual_Buck/Inc/main.h                  Header for main.c module  
  - HRTIM/HRTIM_Dual_Buck/Src/stm32g4xx_it.c          IT interrupt handlers
  - HRTIM/HRTIM_Dual_Buck/Src/main.c                  Main program
  - HRTIM/HRTIM_Dual_Buck/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - HRTIM/HRTIM_Dual_Buck/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment 

  - This example runs on STM32G474RETx devices.
    
  - This example has been tested with B-G474E-DPOW1 board and can be
    easily tailored to any other supported device and development board.

  - B-G474E-DPOW1 Set-up: PA15 must be briefly tied to ground to simulate a fault event

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
