/**
  @page PWR_EnterStandbyMode PWR standby example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_EnterStandbyMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR STANDBY mode example.
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

How to enter the Standby mode and wake up from this mode by using an external 
reset or a wakeup interrupt.

In the associated software, the system clock is set to 170 MHz.
An EXTI line is connected to the User push-button through PC.13 and configured to generate an 
interrupt on falling edge upon key press.
LED2 toggles in order to indicate that MCU is in RUN mode:
- quickly (each 200ms) if program starts from reset
- slowly (each 500ms) if program wakes up from Standby mode

When a falling edge is detected on the EXTI line, an interrupt is generated. 
In the EXTI handler routine, the wake-up pin LL_PWR_WAKEUP_PIN2 is enabled and the 
corresponding wake-up flag cleared. Then, the system enters Standby mode causing 
LED2 to stop toggling. 

Next, the user can wake-up the system in pressing the User push-button which is 
connected to the wake-up pin LL_PWR_WAKEUP_PIN2.
A falling edge on WKUP pin will wake-up the system from Standby.

Alternatively, an external RESET of the board will lead to a system wake-up as well.

After wake-up from Standby mode, program execution restarts in the same way as 
after a RESET and LED2 restarts toggling.

LED2 is used to monitor the system state as follows:
 - LED2 fast toggling: system in Run mode
 - LED2 slow toggling: system in Run mode after exiting from Standby mode
 - LED2 off : system in Standby mode

These steps are repeated in an infinite loop.

@note System in Standby mode : LED state at this step depends on blinking state at the instant of user button is pressed.

@note To measure the current consumption in Standby mode, remove JP6 jumper 
      and connect an ampere meter to JP6 to measure IDD current.

  #error "Define whether multicore is available on the selected STM32 series"
@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M4 core is no longer clocked during low power mode 
      so debugging features are disabled.


@par Keywords

Power, PWR, Standby mode, Interrupt, EXTI, Wakeup, Low Power, External reset,

@par Directory contents 

  - PWR/PWR_EnterStandbyMode/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStandbyMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_EnterStandbyMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_EnterStandbyMode/Src/stm32g4xx_it.c          Interrupt handlers
  - PWR/PWR_EnterStandbyMode/Src/main.c                  Main program
  - PWR/PWR_EnterStandbyMode/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-G431RB RevC
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-G431RB RevC Set-up
    - LED2 connected to PA.05 pin
    - User push-button connected to pin PC.13 (External line 15 to 10)
    - WakeUp Pin LL_PWR_WAKEUP_PIN2 connected to PC.13

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
