/**
  @page Adafruit_LCD_1_8_SD_Joystick Demo NUCLEO-G431RB RevC
  @verbatim
  ******************** (C) COPYRIGHT 2019 STMicroelectronics *******************
  * @file    Adafruit_LCD_1_8_SD_Joystick/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the NUCLEO-G431RB RevC Demo.
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

@par Demo Description

This demonstration provides a short description of how to use the BSP drivers.
The STM32G4xx_Nucleo offers the Leds and User button services.
The Adafruit (reference 802) offers LCD, Joystick and SD services.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

The program checks the presence of Adafruit shield by reading the state of pin PB0:
1- The shield is available
  The following demo can be accessed by pressing Joystick button:
  - Joystick: Use Joystick buttons to move "x" inside a rectangle displayed on LCD
  - LCD: Several shapes and images are displayed on the LCD
  - SD: SD block erase, write, read and result comparison is performed and status of
      these operations is displayed on LCD.

2- The shield is not available
  LED2 is toggling with a first frequency equal to 1Hz.
  Pressing User button lets LED2 toggling with a second frequency equal to 5Hz.
  Pressing User button again, changes LED2 toggling frequency to 10Hz.
  This is done in an infinite loop.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Demonstrations, Adafruit, Joystick, LCD, SD, BSP, LED

@par Directory contents

  - Adafruit_LCD_1_8_SD_Joystick/Inc/adafruit_802_conf.h      Adafruit shield configuration file
  - Adafruit_LCD_1_8_SD_Joystick/Inc/main.h                   Header for main.c module
  - Adafruit_LCD_1_8_SD_Joystick/Inc/stlogo.h                 ST logo data
  - Adafruit_LCD_1_8_SD_Joystick/Inc/stm32g4xx_hal_conf.h     HAL configuration file
  - Adafruit_LCD_1_8_SD_Joystick/Inc/stm32g4xx_it.h           Interrupt handlers header file
  - Adafruit_LCD_1_8_SD_Joystick/Inc/stm32g4xx_nucleo_conf.h  STM32G4xx nucleo configuration file
  - Adafruit_LCD_1_8_SD_Joystick/Inc/stm32g4xx_nucleo_bus.h   Header for stm32g4xx_nucleo_bus.c module
  - Adafruit_LCD_1_8_SD_Joystick/Src/joystick.c               Joystick module
  - Adafruit_LCD_1_8_SD_Joystick/Src/lcd.c                    LCD module
  - Adafruit_LCD_1_8_SD_Joystick/Src/main.c                   Main program
  - Adafruit_LCD_1_8_SD_Joystick/Src/sd.c                     SD module
  - Adafruit_LCD_1_8_SD_Joystick/Src/stm32g4xx_it.c           Interrupt handlers
  - Adafruit_LCD_1_8_SD_Joystick/Src/stm32g4xx_nucleo_bus.c   STM32G4xx nucleo bus module
  - Adafruit_LCD_1_8_SD_Joystick/Src/system_stm32g4xx.c       STM32G4xx system source file


@par Hardware and Software environment

  - This demonstration has been tested with NUCLEO-G431RB RevC board and STM32G431RB device
    It can be easily tailored to any other supported device and development board.

  - Adafruit 1.8" TFT shield must be connected on CN5, CN6, CN8 and CN9 Arduino connectors,

For more details about the adafruit 1.8" TFT shield, please visit:
http://www.adafruit.com/blog/2012/04/26/new-product-adafruit-1-8-18-bit-color-tft-shield-wmicrosd-and-joystick/


@par How to use it ?

In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example

 */
