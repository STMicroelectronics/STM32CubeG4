/**
  @page BSP Mode example
  
  @verbatim
  ******************** (C) COPYRIGHT 2018 STMicroelectronics *******************
  * @file    BSP/BSP_Test/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the STM32G4xx BSP example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description 

This example provides a short description of how to use the BSP to interface with
the EVAL board 

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

Terminal IO is required to run test examples.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Display, LCD, SD card, SRAM, QSPI, LED, Push Button, JOYstick


@par Directory contents  

  - Example/BSP/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - Example/BSP/Inc/stm32g474e_eval_conf.h  EVAL board configuration file
  - Example/BSP/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - Example/BSP/Inc/main.h                  Header for main.c module  
  - Example/BSP/Src/stm32g4xx_it.c          Interrupt handlers
  - Example/BSP/Src/main.c                  Main program
  - Example/BSP/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xxx devices.
    
  - This example has been tested with STM32G474E-EVAL board and can be
    easily tailored to any other supported device and development board.      

  - STM32G474E-EVAL Set-up :
       - JP10 must be closed

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5) 


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
