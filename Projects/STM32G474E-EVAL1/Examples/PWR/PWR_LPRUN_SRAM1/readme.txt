/**
  @page PWR_LPRUN_SRAM1 Low Power Run Mode Example
  
  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_LPRUN_SRAM1/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Low Power Run Mode example executed from SRAM1
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
 
This example shows how to enter and exit the Low Power Run mode.

In the associated software, the system clock is set to 170 MHz.
The SysTick is programmed to generate an interrupt each 1 ms.

5 seconds after start-up, the system automatically enters LP RUN mode
and LED1 stops toggling.
The User push-button can be pressed at any time to exit from Low Power Run. 
The software then comes back in RUN mode for 5 sec. before automatically 
entering LP RUN mode again. 

LED1 is used to monitor the system state as follows:
 - LED1 toggling : system in RUN mode
 - LED1 off : system in LP RUN mode

These steps are repeated in an infinite loop.

@note To measure the current consumption in LP RUN mode, remove JP6 jumper 
      and connect an amperemeter to JP6 to measure IDD current.     

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

Power, PWR, Low Power, Run mode, Interrupt, Wakeup, External reset, SRAM

@par Directory contents 

  - PWR/PWR_LPRUN_SRAM1/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - PWR/PWR_LPRUN_SRAM1/Inc/stm32g4xx_conf.h         HAL Configuration file
  - PWR/PWR_LPRUN_SRAM1/Inc/stm32g4xx_it.h           Header for stm32g4xx_it.c
  - PWR/PWR_LPRUN_SRAM1/Inc/main.h                   Header file for main.c
  - PWR/PWR_LPRUN_SRAM1/Src/system_stm32g4xx.c       STM32G4xx system clock configuration file
  - PWR/PWR_LPRUN_SRAM1/Src/stm32g4xx_it.c           Interrupt handlers
  - PWR/PWR_LPRUN_SRAM1/Src/main.c                   Main program

@par Hardware and Software environment

  - This example runs on STM32G4xx devices
    
  - This example has been tested with STMicroelectronics STM32G474E-EVAL1 Rev B
    board and can be easily tailored to any other supported device 
    and development board.

  - STM32G474E-EVAL1 Rev B set-up:
    - LED1 connected to PG.09 pin
    - Use the User push-button connected to pin PC.13.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
