/**
  @page FLASH_DualBoot  FLASH Dual Boot example

  @verbatim
  ******************************************************************************
  * @file    FLASH/FLASH_DualBoot/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FLASH Dual boot example.
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

This example guides you through the different configuration steps by mean of HAL API
how to program bank1 and bank2 of the STM32G4xx internal Flash memory mounted on STM32G474E-EVAL1 Rev B
and swap between both of them.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 170 MHz.

Below are the steps to run this example:
1- Select bank2 configuration by commenting FLASH_BANK1 and uncommenting FLASH_BANK2 defines
   in "main.h", and generate its binary (ie: FLASH_DualBoot.bin)

2- Load this binary at the bank2 of the flash(at the address 0x08040000) using
   STM32 CubeProgrammer (www.st.com) or any similar tool.
@note:
 - You can avoid step 1 by directly loading the binary file provided with the example
 - You have to configure your preferred toolchain in order to generate the binary
   file after compiling the project.
 - You can use STM32 CubeProgrammer or any similar tool to initially reset the
   BFB2 bit (disable the dual boot feature).

3- Select bank1 configuration by uncommenting FLASH_BANK1 and commenting FLASH_BANK2 defines
   in "main.h", and run it, this project will be loaded in the bank1 of the flash: at the
   address 0x08000000

4- Click the User push-button to swap between the two banks

- If program in bank1 is selected, a message with a blue text back color will be
  displayed on LCD and LED1 will remain toggling while LED2 is turn on.

- If program in bank2 is selected, a message with a red text back color will be
  displayed on LCD and LED2 will remain toggling while LED1 is turn on.

- If error occurs LED3 is turn on.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@note If Bootloader version is lower than V13.4 (0xD4) then the example might not work as expected due to known limitation on Bootloader.
      The Bootloader version can be found in the address 0x1FFF6FFE and should be 0xD4 or higher for the example to work correctly.

@par Keywords

Memory, Flash, Dual boot, Dual Bank, Execute, Binary, Option bytes

@par Directory contents

  - FLASH/FLASH_DualBoot/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - FLASH/FLASH_DualBoot/Inc/stm32g4xx_hal_conf.h        HAL Configuration file
  - FLASH/FLASH_DualBoot/Inc/stm32g4xx_it.h              Header for stm32g4xx_it.c
  - FLASH/FLASH_DualBoot/Inc/main.h                      Header for main.c module
  - FLASH/FLASH_DualBoot/Src/stm32g4xx_it.c              Interrupt handlers
  - FLASH/FLASH_DualBoot/Src/main.c                      Main program
  - FLASH/FLASH_DualBoot/Src/stm32g4xx_msp_hal.c         MSP initialization and de-initialization
  - FLASH/FLASH_DualBoot/Src/system_stm32g4xx.c          STM32G4xx system clock configuration file
  - FLASH/FLASH_DualBoot/Binary/FLASH_DualBoot.bin       Binary file to load at bank2

@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.

  - This example has been tested with STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
