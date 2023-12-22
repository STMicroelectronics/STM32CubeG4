/**
  @page RTC_ExitStandbyWithWakeUpTimer_Init RTC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_ExitStandbyWithWakeUpTimer_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC example.
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

Configuration of the RTC to wake up from Standby mode 
using the RTC Wakeup timer. The peripheral initialization uses LL unitary service 
functions for optimization purposes (performance and size).
  
In this example, after start-up, SYSCLK is configured to the max frequency using
the PLL with HSI as clock source.

   @note LSI oscillator clock is used as RTC clock source by default.
      The user can use also LSE as RTC clock source. The user uncomment the adequate 
      line on the main.c file.
      @code
        #define RTC_CLOCK_SOURCE_LSI  
        /* #define RTC_CLOCK_SOURCE_LSE */
      @endcode
      LSI oscillator clock is delivered by a 32 kHz RC.
      LSE (when available on board) is delivered by a 32.768 kHz crystal.

   @note RTC wake up timer is one-second resolution based due to 1Hz internal frequency configuration.

Example execution:
   - 1st execution of the system, LED2 is quickly blinking (every 200ms).
   - RTC wakeup timer is configured to 5 seconds
   - Press the User push-button:
     * System enters in standby mode (LED2 is switched off)
   - After 5 seconds, system resumes from standby mode, then LED2 is slowly blinking (every 500ms).

   - LED2 is toggling every 1 second: This indicates that the system generates an error.



@par Keywords

System, RTC, RTC Wakeup timer, Standby mode, LSI, LSE, Interrupt,

@par Directory contents 

  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Inc/main.h                  Header for main.c module
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/stm32g4xx_it.c          Interrupt handlers
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/main.c                  Main program
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.
    
  - This example has been tested with NUCLEO-G431RB RevC board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
