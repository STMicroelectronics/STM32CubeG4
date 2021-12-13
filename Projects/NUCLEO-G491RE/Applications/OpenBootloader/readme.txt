/**
  @page OpenBootloader Open Bootloader IAP application
 
  @verbatim
  ******************************************************************************
  * @file    OpenBootloader/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the consumer example.
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

@par Application Description

This application exploits OpenBootloader Middleware to demonstrate how to develop an IAP application
and how use it.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 72 MHz.

Then, these protocols and memories interfaces are configured
  - USART1
  - I2C2
  - FLASH interface
  - RAM interface
  - Option bytes interface

Then, the OpenBootloader application will wait for incoming communication on one of the supported protocols.

Once a communication is established through one of the supported protocols,
the OpenBootloader application will wait for commands sent by the host.

@note In case of Mass Erase operation, the OpenBootloader FLASH area must be protected otherwise the OpenBootloader
      will be erased.

@par Keywords

Middleware, OpenBootloader, USART, I2C

@par Directory contents
     - Core/Src/main.c                                    Main program file
     - Core/Src/stm32g4xx_hal_msp.c                       MSP Initialization file
     - Core/Src/stm32g4xx_it.c                            Interrupt handlers file
     - Core/Src/system_stm32g4xx.c                        STM32G4xx system clock configuration file
     - Core/Inc/main.h                                    Main program header file
     - Core/Inc/stm32g4xx_hal_conf.h                      HAL Library Configuration file
     - Core/Inc/stm32g4xx_it.h                            Interrupt handlers header file
     - OpenBootloader/App/app_openbootloader.c            OpenBootloader application entry point
     - OpenBootloader/App/app_openbootloader.h            Header for OpenBootloader application entry file
     - OpenBootloader/Target/common_interface.c           Contains common functions used by different interfaces
     - OpenBootloader/Target/common_interface.h           Header for common functions file
     - OpenBootloader/Target/i2c_interface.c              Contains I2C interface
     - OpenBootloader/Target/i2c_interface.h              Header of I2C interface file
     - OpenBootloader/Target/flash_interface.c            Contains FLASH interface
     - OpenBootloader/Target/flash_interface.h            Header of FLASH interface file
     - OpenBootloader/Target/iwdg_interface.c             Contains IWDG interface
     - OpenBootloader/Target/iwdg_interface.h             Header of IWDG interface file
     - OpenBootloader/Target/openbootloader_conf.h        Header file that contains OpenBootloader HW dependent configuration
     - OpenBootloader/Target/optionbytes_interface.c      Contains OptionBytes interface
     - OpenBootloader/Target/optionbytes_interface.h      Header of OptionBytes interface file
     - OpenBootloader/Target/ram_interface.c              Contains RAM interface
     - OpenBootloader/Target/ram_interface.h              Header of RAM interface file
     - OpenBootloader/Target/usart_interface.c            Contains USART interface
     - OpenBootloader/Target/usart_interface.h            Header of USART interface file

@par Hardware and Software environment

  - This application runs on STM32G491RE devices.

  - This example has been tested with a NUCLEO-G491RE board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G491RE set-up to use I2C:
    - To use the I2C2 for communication you have to connect:
      - SCL pin of your host adapter to PC4 pin (Arduino connector CN9 pin D1, Morpho connector CN10 pin 35) of the NUCLEO-G491RE board
      - SDA pin of your host adapter to PA8 pin (Arduino connector CN9 pin D7, Morpho connector CN10 pin 23) of the NUCLEO-G491RE board
    - Set I2C address to 0xBE for the device, set I2C address to 0x5F for the host
    - The I2C2 own address is set to 0x5F and can be easy customized by updating the defined value "I2C_ADDRESS" in interface_conf.h file

  - NUCLEO-G491RE set-up to use USART:
    - To use the USART1 for communication you have to connect:
      - Tx pin of your host adapter to PA10 pin (Arduino connector CN9 pin D2, Morpho connector CN10 pin 33) of the NUCLEO-G491RE board
      - Rx pin of your host adapter to PA9 pin (Arduino connector CN5 pin D8, Morpho connector CN10 pin 21) of the NUCLEO-G491RE board

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
  - Run STM32CubeProgrammer and connect to OpenBootloader using USART1
    Or
  - Run STM32CubeProgrammer and connect to OpenBootloader through STLINK V3 using I2C2

 */
