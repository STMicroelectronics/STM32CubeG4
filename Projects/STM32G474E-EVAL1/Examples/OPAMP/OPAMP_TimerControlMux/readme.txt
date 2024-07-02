/**
  @page OPAMP_TimerControlMux OPAMP example

  @verbatim
  ******************************************************************************
  * @file    Examples/OPAMP/OPAMP_TimerControlMux/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the OPAMP_TimerControlMux example.
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

@par Example Brief 

This example shows how to use the timer controlled multiplexer mode of the OPAMP.

@par Example Description

This mode allows upon a timer trigger to change OPAMP configuration from a primary
one to a secondary one. Possibilities are as follow:

Primary configuration is standalone:
- Secondary is standalone with possibility to change either one or both inputs

Primary configuration is follower or PGA:
- Secondary can be follower with same or different non inverting input
- Secondary can be PGA with same or different non inverting input

This example is configuring OPAMP4 as follow:
- Primary configuration is follower with non inverting input on DAC4
generating a triangle wave.
- Secondary configuration is follower with non inverting input on PB13
connected to potentiometer RV2
- TIM1 is used to trig the timer controlled multiplexer mode

The OPAMP output (PB12 - pin 16 on connector CN6) should replicate alternatively
both input signals (triangle wave and potentiometer value).

- Connection needed:
 - Connect a scope on OPAMP output (PB12 - pin 16 on connector CN6) to observe
 timer controlled multiplexer effects.
 - Connect jumper JP5 on 2-3 position (LDR)
 - Connect a wire between JP5 pin 1 and PB13 (pin 13 on connector CN6)

STM32G474E-EVAL1 Rev B board LED is be used to monitor the program execution status:
 - Normal operation: LED1 is turned-on
 - Error: In case of error, LED1 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, OPAMP, Timer controlled multiplexer

@par Directory contents 

  - OPAMP/OPAMP_TimerControlMux/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - OPAMP/OPAMP_TimerControlMux/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - OPAMP/OPAMP_TimerControlMux/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - OPAMP/OPAMP_TimerControlMux/Inc/main.h                  Header for main.c module  
  - OPAMP/OPAMP_TimerControlMux/Src/stm32g4xx_it.c          Interrupt handlers
  - OPAMP/OPAMP_TimerControlMux/Src/main.c                  Main program
  - OPAMP/OPAMP_TimerControlMux/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - OPAMP/OPAMP_TimerControlMux/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment  
  
  - This example runs on STM32G474xx devices.
    
  - This example has been tested with STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.

- Connect jumper JP5 on 2-3 position (LDR)
 - Connect a wire between JP5 pin 1 and PB13 (pin 13 on connector CN6)

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  
 */

