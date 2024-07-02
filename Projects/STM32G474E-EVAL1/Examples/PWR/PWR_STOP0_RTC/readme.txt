/**
  @page PWR_STOP0_RTC  Power Stop 0 RTC Mode example
  
  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_STOP0_RTC/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR STOP1 RTC example.
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

This example shows how to enter Stop 0 mode and wake up from this mode using 
an interrupt from RTC Wake-up Timer. 
It allows to measure the current consumption in STOP 0 mode with RTC enabled.

In the associated software, the system clock is set to 170 MHz and the SysTick is 
programmed to generate an interrupt each 1 ms.
The Low Speed Internal (LSI) clock is used as RTC clock source by default.
EXTI_Line20 is internally connected to the RTC Wakeup event.


The system automatically enters STOP 0 mode 5 sec. after start-up.
The RTC wake-up is configured to generate an interrupt on rising edge about 33 sec. afterwards.
Current consumption in STOP 0 mode with RTC feature enabled can be measured during that time.
More than half a minute is chosen to ensure current convergence to its lowest operating point.

After wake-up from STOP 0 mode, program execution is resumed.

Two leds LED1 and LED2 are used to monitor the system state as following:
 - LED2 ON: configuration failed (system will go to an infinite loop)
 - LED1 toggling: system in RUN mode
 - LED1 off : system in STOP 0 mode

These steps are repeated in an infinite loop.
 
@note To measure the current consumption in STOP 0 mode, remove JP13 jumper 
      and connect an amperemeter to JP13 to measure IDD current.     

@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M4 core is no longer clocked during low power mode 
       so debugging features are disabled.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.
      
@par Keywords

Power, PWR, Stop 1 mode, Interrupt, EXTI, Wakeup, Low Power, RTC, External reset

@par Directory contents 

  - PWR/PWR_STOP0_RTC/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - PWR/PWR_STOP1_RTC/Inc/stm32g4xx_conf.h         HAL Configuration file
  - PWR/PWR_STOP1_RTC/Inc/stm32g4xx_it.h           Header for stm32g4xx_it.c
  - PWR/PWR_STOP1_RTC/Inc/main.h                   Header file for main.c
  - PWR/PWR_STOP1_RTC/Src/system_stm32g4xx.c       STM32G4xx system clock configuration file
  - PWR/PWR_STOP1_RTC/Src/stm32g4xx_it.c           Interrupt handlers
  - PWR/PWR_STOP1_RTC/Src/main.c                   Main program
  - PWR/PWR_STOP1_RTC/Src/stm32g4xx_hal_msp.c      HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32G4xx devices
    
  - This example has been tested with STMicroelectronics STM32G474E-EVAL1 Rev B
    board and can be easily tailored to any other supported device 
    and development board.

  - STM32G474E-EVAL1 Rev B Set-up
    - Use LED1 and LED2 connected respectively to PG.09 and MFX6 pins


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
