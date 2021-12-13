/**
  @page Led_Jumper Demo NUCLEO-G431KB Rev A

  @verbatim
  ********************* COPYRIGHT(c) 2019 STMicroelectronics *******************
  * @file    Led_Jumper/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the NUCLEO-G431KB Rev A Demo.
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

@par Demo Description

This demonstration provides a short description of how to use the BSP drivers.
The STM32G4xx_Nucleo_32 offers the Leds' services.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

The program checks the presence of jumper between CN4 pin 5 and CN4 pin 4 by
reading the state of pin PA.12.
When jumper is in place, green LED2 is toggling with a frequency equal to ~1Hz.
When jumper is removed, green LED2 is toggling with a frequency equal to ~5Hz.
This is done in an infinite loop.


@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@par Keywords

Demonstrations, Configuration, Initialization, Reset, STM32 Cortex-M device, Jumper, LED

@par Directory contents

  - Led_Jumper/Src/main.c                     Main program
  - Led_Jumper/Src/system_stm32g4xx.c         STM32G4xx system clock configuration file
  - Led_Jumper/Src/stm32g4xx_it.c             Interrupt handlers
  - Led_Jumper/Inc/main.h                     Main program header file
  - Led_Jumper/Inc/stm32g4xx_hal_conf.h       HAL Configuration file
  - Led_Jumper/Inc/stm32g4xx_it.h             Interrupt handlers header file
  - Led_Jumper/Inc/stm32g4xx_nucleo_32_conf.h STM32G4xx nucleo 32 configuration file


@par Hardware and Software environment

  - This demonstration has been tested with NUCLEO-G431KB Rev A board and STM32G431KB device
    It can be easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
