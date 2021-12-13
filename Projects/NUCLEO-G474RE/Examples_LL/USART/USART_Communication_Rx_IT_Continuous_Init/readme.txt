/**
  @page USART_Communication_Rx_IT_Continuous_Init USART Receiver example (IT Mode, Continuous reception)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_Communication_Rx_IT_Continuous_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_Communication_Rx_IT_Continuous_Init example.
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

This example shows how to configure GPIO and USART peripheral for continuously receiving characters
from HyperTerminal (PC) in Asynchronous mode using Interrupt mode. Peripheral initialization is 
done using LL unitary services functions for optimization purpose (performance and size).

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI. 

Example execution:
After startup from reset and system configuration, LED2 is blinking quickly.
On press on push button, USART RX Not Empty interrupt is enabled.
When character is received on USART Rx line, a RXNE interrupt occurs. 
Received data is read from USART RDR register and stored in user buffer.
A double buffer is available for received data, allowing continuous reception.
User could process data received in Buffer A, while buffer B is used for reception. 
When buffer B is full, it could be handled by user, while buffer A becomes active buffer for next reception, and so on.
Each time a reception buffer is full, user data process callback is called.
Data processing consists in echoing data buffer content on PC Com port, and making LED2 toggle.
In case of errors, LED2 is blinking.

@par Keywords

Connectivity, USART, Asynchronous mode, RS-232, Baud rate, Interrupt, HyperTerminal, Reception.

@par Directory contents 

  - USART/USART_Communication_Rx_IT_Continuous_Init/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - USART/USART_Communication_Rx_IT_Continuous_Init/Inc/main.h                        Header for main.c module
  - USART/USART_Communication_Rx_IT_Continuous_Init/Inc/stm32_assert.h                Template file to include assert_failed function
  - USART/USART_Communication_Rx_IT_Continuous_Init/Src/stm32g4xx_it.c          Interrupt handlers
  - USART/USART_Communication_Rx_IT_Continuous_Init/Src/main.c                        Main program
  - USART/USART_Communication_Rx_IT_Continuous_Init/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474RETx devices.
    
  - This example has been tested with NUCLEO-G474RE RevC board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G474RE RevC Set-up
    Connect USART1 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter) :
    - Connect STM32 MCU board USART1 TX pin (GPIO PA.09 connected to pin 21 in CN10)
      to PC COM port RX signal
    - Connect STM32 MCU board USART1 RX pin (GPIO PA.10 connected to pin 33 in CN10)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control).

  - Launch the program. Enter characters on PC communication SW side.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
