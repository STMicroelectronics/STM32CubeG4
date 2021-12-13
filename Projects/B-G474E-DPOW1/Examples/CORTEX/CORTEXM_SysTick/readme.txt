    /**
  @page CORTEXM_SysTick CORTEXM SysTick example

  @verbatim
  ******************************************************************************
  * @file    CORTEX/CORTEXM_SysTick/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the CortexM SysTick example.
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
  
How to use the default SysTick configuration with a 1 ms timebase to toggle LEDs.

  A "HAL_Delay" function is implemented based on the SysTick end-of-count event.
  Four LEDs are toggled with a timing defined by the HAL_Delay function.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, Cortex, Time base, Systick, HCLK 

@par Directory contents 

  - CORTEX/CORTEXM_SysTick/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - CORTEX/CORTEXM_SysTick/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - CORTEX/CORTEXM_SysTick/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEXM_SysTick/Src/stm32g4xx_it.c          Interrupt handlers
  - CORTEX/CORTEXM_SysTick/Src//stm32g4xx_hal_msp.c     HAL MSP file
  - CORTEX/CORTEXM_SysTick/Src/main.c                  Main program
  - CORTEX/CORTEXM_SysTick/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment 

  - This example runs on STM32G474RETx devices.
    
  - This example has been tested with B-G474E-DPOW1 board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 
In order to make the program work, you must do the following :
- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the example

 */
