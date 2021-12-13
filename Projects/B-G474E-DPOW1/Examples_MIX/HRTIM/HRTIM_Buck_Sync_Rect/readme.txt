/**
  @page HRTIM_Buck_Sync_Rect HRTIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_MIX/HRTIM/HRTIM_Buck_Sync_Rect/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the HRTIM_Buck_Sync_Rect example.
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
  This example shows how to configure the HRTIM to control a buck converter
  with synchronous rectification.
  This example uses the STM32G4xx HRTIM HAL and LL API.

  This is done using timer unit C and TC1/TC2 outputs (resp. PB12 and PB13).
  The duty cycle on TC1 is continuously varied in an interrupt service routine
  generated on repetition event, to mimic a real converter management.
  The FAULT2 input is enabled on PA15 (active low) to demonstrate PWM shut down
  (low level sensitive), for both TC1 and TC2.
  When the fault is triggered (PA15 input connected to GND), TC1 and TC2 signals
  are shut-down. The system can be re-armed by pressing the user button.

  LEDs are indicating the following:
  Green LED: blinks during normal operation
  Red LED: blinks when FAULT is triggered
  Orange LED: indicates the occurrence and duration of the PWM update ISR.

  The ADC is configured to have conversions triggered in the middle of the
  converter ON time, on PA1 (BUCKBOOST_VIN) and PA3 (BUCKBOOST_VOUT) inputs.

  To run the demo, the Vin input of the BUCK-BOOST converter must be connected
  to the 5V_O pin on the CN9 connector or to an external power supply. The 
  resulting voltage is available on Vout pin.
  NB: to have the BOOST stage bypassed, the PB15 pin must be forced at 1 (to
  have the T6 PMOS switched ON).

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

HRTIM, Timers, Buck converter, Synchronous rectification, PWM , Low level, ADC, Duty cycle. 

@par Directory contents

  - HRTIM/HRTIM_Buck_Sync_Rect/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - HRTIM/HRTIM_Buck_Sync_Rect/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - HRTIM/HRTIM_Buck_Sync_Rect/Inc/main.h                  Header for main.c module
  - HRTIM/HRTIM_Buck_Sync_Rect/Src/stm32g4xx_it.c          IT interrupt handlers
  - HRTIM/HRTIM_Buck_Sync_Rect/Src/main.c                  Main program
  - HRTIM/HRTIM_Buck_Sync_Rect/Src/system_stm32g4xx.c      STM32G4xx system source file


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

 */
