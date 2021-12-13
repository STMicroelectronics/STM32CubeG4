/**
  @page USART_Communication_TxRx_DMA_Init USART Transmitter/Receiver example (DMA mode)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_Communication_TxRx_DMA_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_Communication_TxRx_DMA_Init example.
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

This example shows how to configure GPIO and USART peripheral
to send characters asynchronously to/from an HyperTerminal (PC) in
DMA mode. This example is based on STM32G4xx USART LL API. Peripheral
initialization is done using LL unitary services functions for optimization
purpose (performance and size).

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI. 

Example execution:
After startup from reset and system configuration, LED2 is blinking quickly and wait User push-button action.
USART and DMA are configured.
On press on push button , TX DMA transfer to PC is initiated. 
RX DMA transfer is also ready to retrieve characters from PC ("END" string is expected to be entered by user on PC side (HyperTerminal).
In case of both transfers successfully completed, LED2 is turned on.
In case of errors, LED2 is blinking (1sec period).

@par Keywords

Connectivity, USART, Asynchronous mode, RS-232, Baud rate, DMA, HyperTerminal, Transmission.

@par Directory contents 

  - USART/USART_Communication_TxRx_DMA_Init/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - USART/USART_Communication_TxRx_DMA_Init/Inc/main.h                  Header for main.c module
  - USART/USART_Communication_TxRx_DMA_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_Communication_TxRx_DMA_Init/Src/stm32g4xx_it.c          Interrupt handlers
  - USART/USART_Communication_TxRx_DMA_Init/Src/main.c                  Main program
  - USART/USART_Communication_TxRx_DMA_Init/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.
    
  - This example has been tested with NUCLEO-G431RB RevC board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G431RB RevC Set-up
    Connect USART1 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter) :
    - Connect STM32 MCU board USART1 TX pin (GPIO PA.09 connected to pin 21 in CN10)
      to PC COM port RX signal
    - Connect STM32 MCU board USART1 RX pin (GPIO PA.10 connected to pin 33 in CN10)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control). 

  - Launch the program. Press on User push button on board to initiate data transfer.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
