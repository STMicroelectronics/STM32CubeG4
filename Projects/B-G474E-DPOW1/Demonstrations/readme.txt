/**
  @page Demo   Demo B-G474E-DPOW1

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @author  MCD Application Team
  * @brief   Description of B-G474E-DPOW1 Demo
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Demo Description

This demonstration firmware is based on STM32Cube. It helps you to discover
STM32 Cortex-M devices that can be plugged on a STM32 Discovery board.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock
(SYSCLK) to run at 170 MHz.

This demonstration cover followings use cases :
- 4 LEDs demo
- RGB LED Color demo
- RGB LED White demo
- Buck/Boost demo

For a detailed description of the demonstration, please refer to Application Note AN5345
Document is available here : http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00624818.pdf

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process,
      then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@par Hardware and Software environment

  - This demonstration runs on B-G474E-DPOW1 board (MB11428 revB)

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
