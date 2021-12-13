/**
  @page USART_WakeUpFromStop_Init USART example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_WakeUpFromStop_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_WakeUpFromStop_Init USART example.
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
  
Configuration of GPIO and USART1 peripherals to allow the characters received on USART_RX pin to wake up the MCU from low-power mode. 
This example is based on the STM32G4xx USART1 LL API. The peripheral 
initialization uses LL unitary service functions for optimization purposes (performance and size).

USART1 Peripheral is configured in asynchronous mode (9600 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
USART1 Clock is based on HSI.

Example execution:
After startup from reset and system configuration, LED2 is blinking quickly during 3 sec,
then MCU  enters "Stop 0" mode (LED2 off).
On first  character reception by the USART1 from PC Com port (ex: using HyperTerminal)
after "Stop 0" Mode period, MCU wakes up from "Stop 0" Mode.
Received character value is checked :
- On a specific value ('S' or 's'), LED2 is turned On and program ends.
- If different from 'S' or 's', program performs a quick LED2 blinks during 3 sec and
  enters again "Stop 0" mode, waiting for next character to wake up.

In case of errors, LED2 is slowly blinking (1 sec period).

@par Keywords

Connectivity, USART, Asynchronous mode, RS-232, baud rate, Reception, Transmission, HyperTerminal, Low-power mode, Stop mode.

@par Directory contents 

  - USART/USART_WakeUpFromStop_Init/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - USART/USART_WakeUpFromStop_Init/Inc/main.h                  Header for main.c module
  - USART/USART_WakeUpFromStop_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_WakeUpFromStop_Init/Src/stm32g4xx_it.c          Interrupt handlers
  - USART/USART_WakeUpFromStop_Init/Src/main.c                  Main program
  - USART/USART_WakeUpFromStop_Init/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.
    
  - This example has been tested with NUCLEO-G431RB RevC board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G431RB RevC Set-up
    - Connect STM32 MCU board USART1 TX pin (GPIO PA9 connected to PA.09 connected to pin 21 in CN10)
      to PC COM port RX signal
    - Connect STM32 MCU board USART1 RX pin (GPIO PA10 connected to PA.10 connected to pin 33 in CN10)
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
