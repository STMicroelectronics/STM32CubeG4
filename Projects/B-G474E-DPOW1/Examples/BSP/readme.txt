/**
  @page BSP Mode example

  @verbatim
  ******************** (C) COPYRIGHT 2019 STMicroelectronics *******************
  * @file    readme.txt
  * @author  MCD Application Team
  * @brief   Description of the B-G474E-DPOW1 BSP example.
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

This example provides a short description of how to use the JOYSTICK button and leds.
The program starts switching on/off and toggling LEDs

Then the program is testing the Joystick in GPIO mode.

Pressing the direction buttons of the JOYstick (B2) will switch on and off  the corresponding LED (toggling).
Pressing the SEL button of the Joystick (B2) will switch off the LEDs and deinit the Joystick.

Then the program is testing the Joystick in EXTI (interrupt mode) mode.
Pressing the direction buttons of the JOYstick (B2) will switch on and off the corresponding LED (toggling).
Pressing the SEL button of the Joystick (B2) will switch on all the LEDs.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Joystick, LED
@par Directory contents

  - Misc/Init/Inc/stm32g4xx_hal_conf.h      HAL configuration file
  - Misc/Init/Inc/b_g474e_dpow1.h           B-G474E-DPOW1 board configuration
  - Misc/Init/Inc/stm32g4xx_it.h            Interrupt handlers header file
  - Misc/Init/Inc/main.h                    Header for main.c module
  - Misc/Init/Src/stm32g4xx_it.c            Interrupt handlers
  - Misc/Init/Src/main.c                    Main program
  - Misc/Init/Src/system_stm32g4xx.c        STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xxx devices.

  - This example has been tested with B-G474E-DPOW1 board and can be
    easily tailored to any other supported device and development board.

  - B-G474E-DPOW1 Set-up :
       - JP4 must be installed

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5)


 */
