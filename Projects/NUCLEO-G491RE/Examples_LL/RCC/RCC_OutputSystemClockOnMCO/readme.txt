/**
  @page RCC_OutputSystemClockOnMCO RCC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RCC/RCC_OutputSystemClockOnMCO/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RCC example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

Configuration of MCO pin (PA8) to output the system clock.

At start-up, User push-button and MCO pin are configured. The program configures SYSCLK
to the max frequency using the PLL with HSI as clock source.

The signal on PA8 (or pin 23 of CN10 connector) can be monitored with an oscilloscope
to check the different MCO configuration set at each User push-button press.
Different configuration will be observed
 - HSI frequency with frequency value around @16MHz.
 - PLLCLK frequency value (= SYSCLK) divided by 16, hence around @10.6MHz.
 - HSE frequency value divided by 2, hence around @12MHz.

When user press User push-button, a LED2 toggle is done to indicate a change in MCO config.

@par Keywords

System, RCC, PLL, HSI, PLLCLK, SYSCLK, HSE, Clock, Oscillator


@par Directory contents 

  - RCC/RCC_OutputSystemClockOnMCO/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - RCC/RCC_OutputSystemClockOnMCO/Inc/main.h                  Header for main.c module
  - RCC/RCC_OutputSystemClockOnMCO/Inc/stm32_assert.h          Template file to include assert_failed function
  - RCC/RCC_OutputSystemClockOnMCO/Src/stm32g4xx_it.c          Interrupt handlers
  - RCC/RCC_OutputSystemClockOnMCO/Src/main.c                  Main program
  - RCC/RCC_OutputSystemClockOnMCO/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G491RETx devices.
    
  - This example has been tested with NUCLEO-G491RE RevA board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G491RE RevA Set-up
    - Connect the MCO pin to an oscilloscope to monitor the different waveforms:
      - PA.08: connected to pin 23 of CN10 connector

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
