/**
  @page HRTIM_Buck_Boost HRTIM example

  @verbatim
  ******************************************************************************
  * @file    Examples_MIX/HRTIM/HRTIM_Buck_Boost/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the HRTIM_Buck_Boost example.
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
  This example shows how to configure the HRTIM to control a non-inverting
  buck-boost converter timer. 
  This example uses the STM32G4xx HRTIM HAL and LL APIs.

  This is done using timer unit C and D and TC1/TC2/TD1/TD2 outputs
  (resp. PB12, PB13, PB14, PB15).

  The FAULT2 input is enabled on PA15 (active low) to demonstrate PWM shut down
  (low level sensitive), for all outputs.
  When the fault is triggered (PA15 input connected to GND), TC1, TC2, TD1, TD2
  signals are shut-down. The system can be re-armed by pressing the user button.

  The ADC is configured to have conversions triggered in the middle of the
  converter ON time, on PA1 (BUCKBOOST_VIN) and PA3 (BUCKBOOST_VOUT) inputs.

  To run the demo, the Vin input pin of the BUCK-BOOST converter must be connected
  to the 5V_O supply on the CN9 connector or on an external power supply. The 
  resulting voltage is available on Vout pin.

  The demo starts in BUCK mode, and the duty cycle is slowly adjusted in the
  TIMC IRQ handler to have Vout continuously varying below Vin value.
  If the push-button is pressed and the voltage is below 5V, the boost mode is
  enabled (this is to prevent exceeding the discovery kit max output voltage.
  The voltage is increased above Vin value (with a fixed duty cycle).
  If the push-button is pressed again, the output capacitor is first de-energized
  down to 2.5V before re-enabling the buck mode.

  LEDs are indicating the following:
  Green LED4: blinks during BUCK operation
  Orange LED3: blinks during BOOST operation
  Red LED5: blinks when FAULT is triggered

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

HRTIM, Timers, Buck mode, Boost mode, PWM , Low level, User button, ADC, Duty cycle. 

@par Directory contents

  - HRTIM/HRTIM_Buck_Boost/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - HRTIM/HRTIM_Buck_Boost/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - HRTIM/HRTIM_Buck_Boost/Inc/main.h                  Header for main.c module
  - HRTIM/HRTIM_Buck_Boost/Src/stm32g4xx_it.c          IT interrupt handlers
  - HRTIM/HRTIM_Buck_Boost/Src/main.c                  Main program
  - HRTIM/HRTIM_Buck_Boost/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - HRTIM/HRTIM_Buck_Boost/Src/system_stm32g4xx.c      STM32G4xx system source file


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
