/**
  @page BSP Mode example

  @verbatim
  ******************** (C) COPYRIGHT 2019 STMicroelectronics *******************
  * @file    BSP/BSP_Test/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the STM32G4xx BSP example.
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

This example provides a short description of how to use the BSP to interface with
the EVAL1 board

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

Terminal IO is required to view messages of examples.
Once the message "Press User button to start :
                       LED example" is displayed on the screen, press user button to start running the first demo

1_ Led_demo: Every time the message "If result is OK press JOY_RIGHT (pass), otherwise press JOY_LEFT (fail)" is displayed on the Terminal I/O,
             press joy_right if the four leds are toggling correctly, else, press joy_left.

Once the message "Press User button to start :
                       BUTTON example" is displayed on the screen, press user button.

2_ Button_demo: Press user button twice following the messages displayed in the Terminal I/O.
Once the message "Press User button to start :
                       IO example" is displayed on the screen, press user button.

3_ Io_demo: Every time the message "If result is OK press JOY_RIGHT (pass), otherwise press JOY_LEFT (fail)" is displayed on the Terminal I/O,
            press joy_right if the led4 (blue) is toggling correctly, else, press joy_left.
            press joy_sel button once the message "Please press JOY_SEL" is displayed on the Terminal I/O.

4_ Lcd_demo: Every time the message "If result is OK press JOY_RIGHT (pass), otherwise press JOY_LEFT (fail)' is displayed on the Terminal I/O,
             press joy_right if the screen orientation is correct, else, press joy_left.
Once the message "Press User button to start :
                       AUDIO PLAY example" is displayed on the screen, press user button.

5_ AudioPlay_demo: press joy_sel key to play and pause the audio
                   press joy up/down keys to increase/decrease volume
                   press joy right/left keys to increase/decrease frequency
                   press user button again to quit AUDIO PLAY demo

6_ Bus_demo: The message "TEST OF BUS Passed" is displayed on the Terminal I/O.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note When running the audio, the sample file (audio8kStereo16bps.bin) must be loaded in the flash memory
      at address 0x08023000 as specified in the main.h
      The recorded file is stored in the flash memory at the address 0x08024000 as specified in the main.h

@par Keywords

Display, LCD, SD card, SRAM, QUADSPI, LED, Push Button, Joystick, audio


@par Directory contents

  - Example/BSP/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - Example/BSP/Inc/stm32g474e_eval_conf.h  EVAL1 board configuration file
  - Example/BSP/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - Example/BSP/Inc/main.h                  Header for main.c module
  - Example/BSP/Src/stm32g4xx_it.c          Interrupt handlers
  - Example/BSP/Src/main.c                  Main program
  - Example/BSP/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xxx devices.

  - This example has been tested with STM32G474E-EVAL1 board and can be
    easily tailored to any other supported device and development board.

  - STM32G474E-EVAL1 Set-up :
       - JP10 must be closed

- When resorting to EWARM IAR IDE:
Command Code is displayed on debugger as follows: View --> Terminal I/O

- When resorting to MDK-ARM KEIL IDE:
Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer
The Audio play demo is not supported on MDK-ARM IDE.

- When resorting to STM32CubeIDE:
Command Code is displayed on debugger as follows: Window--> Show View--> Console.
In Debug configuration :
- Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
- Window\Startup,add the command "monitor arm semihosting enable"

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5)


 */
