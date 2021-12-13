/**
  @page USART_WakeUpFromStop1 USART example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_WakeUpFromStop1/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_WakeUpFromStop1 USART example.
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

Configuration of GPIO and USART peripherals 
to receive characters on USART_RX pin and wake up the MCU from low-power mode. This example is based on 
the STM32G4xx USART LL API. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

USART Peripheral is configured in asynchronous mode (9600 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
USART Clock is based on HSI.

Example execution:
After startup from reset and system configuration, LED2 is blinking quickly during 3 sec,
then MCU  enters "Stop 1" mode (LED2 off).
On first  character reception by the USART from PC Com port (ex: using HyperTerminal)
after "Stop 1" Mode period, MCU wakes up from "Stop 1" Mode.
Received character value is checked :
- On a specific value ('S' or 's'), LED2 is turned On and program ends.
- If different from 'S' or 's', program performs a quick LED2 blinks during 3 sec and
  enters again "Stop 1" mode, waiting for next character to wake up.

In case of errors, LED2 is slowly blinking (1 sec period).

@par Keywords

Connectivity, USART, Asynchronous mode, RS-232, baud rate, Reception, Transmission, HyperTerminal, Low-power mode, Stop mode.

@par Directory contents 

  - USART/USART_WakeUpFromStop1/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - USART/USART_WakeUpFromStop1/Inc/main.h                  Header for main.c module
  - USART/USART_WakeUpFromStop1/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_WakeUpFromStop1/Src/stm32g4xx_it.c          Interrupt handlers
  - USART/USART_WakeUpFromStop1/Src/main.c                  Main program
  - USART/USART_WakeUpFromStop1/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G474xx devices.

  - This example has been tested with NUCLEO-G474RE RevC board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G474RE RevC Set-up
    - Connect STM32 MCU board USART1 TX pin (GPIO PA9 connected to pin 21 of CN10 connector)
      to PC COM port RX signal
    - Connect STM32 MCU board USART1 RX pin (GPIO PA10 connected to pin 33 of CN10 connector)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

    - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
      (9600 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control). 

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
