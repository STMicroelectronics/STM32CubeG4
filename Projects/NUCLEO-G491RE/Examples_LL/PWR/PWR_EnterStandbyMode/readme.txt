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

How to enter the Standby mode and wake up from this mode by using an external 
reset or a wakeup pin.

In the associated software, the system clock is set to 170 MHz.
An EXTI line is connected to the User push-button through PC.13 and configured to generate an 
interrupt on falling edge upon key press.

Request to enter in standby mode:
When a falling edge is detected on the EXTI line, an interrupt is generated. 
In the EXTI handler routine, the wake-up pin LL_PWR_WAKEUP_PIN2 is enabled and the 
corresponding wake-up flag cleared. Then, the system enters Standby mode causing 
LED2 to stop toggling.

Exit from Standby mode:
The user can wake-up the system in pressing the User push-button which is 
connected to the wake-up pin LL_PWR_WAKEUP_PIN2.
A falling edge on WKUP pin will wake-up the system from Standby.

Alternatively, an external reset (reset button on board) will wake-up system from Standby
as well as wake-up pin.

After wake-up from Standby mode, program execution restarts in the same way as 
after a reset and LED2 restarts toggling.
These steps are repeated in an infinite loop.

LED2 is used to monitor the system state as follows:
 - LED2 fast toggling (each 200ms): system in Run mode from reset (power on reset or reset pin while system was in Run mode)
 - LED2 slow toggling (each 500ms): system in Run mode after exiting from Standby mode
 - LED2 off: system in Standby mode

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

  - This example runs on STM32G491RETx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-G491RE RevA
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-G491RE RevA Set-up
    - LED2 connected to PA.05 pin
    - User push-button connected to pin PC.13 (External line 13)
    - WakeUp Pin LL_PWR_WAKEUP_PIN2 connected to PC.13

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
