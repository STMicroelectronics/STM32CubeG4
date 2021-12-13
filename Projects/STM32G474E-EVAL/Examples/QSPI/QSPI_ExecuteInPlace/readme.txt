/**
  @page QSPI_ExecuteInPlace QSPI execute in place example
  
  @verbatim
  ******************************************************************************
  * @file    QSPI/QSPI_ExecuteInPlace/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the code execution from QSPI memory example.
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

This example describes how to execute a part of the code from the QSPI memory. To do this, 
a section is created where the function is stored.

At the startup, the QSPI memory is erased, then the data are copied from the initialization
section of the flash to the QSPI memory. And finally the QSPI is configured in memory-mapped 
mode and the code in executed in a forever loop.

LED1, LED2, LED3 and LED4 toggle in a forever loop.
LED3 is on as soon as an error is returned by HAL API, and no more LED toggles.

In this example, HCLK is configured at 170 MHz.
QSPI prescaler is set to 1, so QSPI frequency is = HCLK/(QSPI_Prescaler+1) = 170 MHz/(1+1)

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Memory, QUADSPI, Erase, Write, Execute, Mapped

@par Directory contents 

  - QSPI/QSPI_ExecuteInPlace/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - QSPI/QSPI_ExecuteInPlace/Inc/stm32g4xx_hal_conf.h HAL configuration file
  - QSPI/QSPI_ExecuteInPlace/Inc/stm32g4xx_it.h       Interrupt handlers header file
  - QSPI/QSPI_ExecuteInPlace/Inc/main.h               Header for main.c module  
  - QSPI/QSPI_ExecuteInPlace/Src/stm32g4xx_it.c       Interrupt handlers
  - QSPI/QSPI_ExecuteInPlace/Src/main.c               Main program
  - QSPI/QSPI_ExecuteInPlace/Src/system_stm32g4xx.c   STM32G4xx system source file
  - QSPI/QSPI_ExecuteInPlace/Src/stm32g4xx_hal_msp.c  HAL MSP file    


@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.

  - This example has been tested with STM32G474E-EVAL Rev B board and can be
    easily tailored to any other supported device and development board.

  - STM32G474E-EVAL Rev B Set-up :
    - Board is configured by default to access QSPI memory

  - A specific region for the QSPI memory has been added in the scatter file 
   (STM32G474QETx_flash.icf for IAR toolchain, STM32G474E-EVAL Rev B.sct for Keil toolchain)
   in order to map functions in this memory

   - The debugger does not work when code is executed from QSPI memory.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
