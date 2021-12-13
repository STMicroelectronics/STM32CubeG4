/**
  @page RCC_UseHSI_PLLasSystemClock RCC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RCC/RCC_UseHSI_PLLasSystemClock/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RCC_UseHSI_PLLasSystemClock example.
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

Modification of the PLL parameters in run time.

In this example, the toggling frequency of the green LED2 depends on the system clock 
frequency and, each time the User push-button is pressed, the PLL switches between two configurations.
This make the LED2 blinking speed to highlight the system clock frequency changes.

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSI as clock source.
Then, when pressing User push-button, an automatic switch is done between PLL with HSI as clock source (SYSCLK
set to 48MHz) and PLL with HSI as clock source (SYSCLK set to 170MHz). 
LED2 will toggle differently between the 2 configurations (quick toggle with SYSCLK configuration at 170MHz).

To detect a problem with PLL configuration, switch USE_TIMEOUT can be enabled. Then in case of issues, 
LED2 will toggle every 1 second.
Note: "uwFrequency" variable can be added in LiveWatch to monitor the system clock frequency.

@par Keywords

System, RCC, PLL, PLLCLK, SYSCLK, HSE, Clock, Oscillator, HSI


@par Directory contents 

  - RCC/RCC_UseHSI_PLLasSystemClock/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - RCC/RCC_UseHSI_PLLasSystemClock/Inc/main.h                  Header for main.c module
  - RCC/RCC_UseHSI_PLLasSystemClock/Inc/stm32_assert.h          Template file to include assert_failed function
  - RCC/RCC_UseHSI_PLLasSystemClock/Src/stm32g4xx_it.c          Interrupt handlers
  - RCC/RCC_UseHSI_PLLasSystemClock/Src/main.c                  Main program
  - RCC/RCC_UseHSI_PLLasSystemClock/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G491RETx devices.
    
  - This example has been tested with NUCLEO-G491RE RevA board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
