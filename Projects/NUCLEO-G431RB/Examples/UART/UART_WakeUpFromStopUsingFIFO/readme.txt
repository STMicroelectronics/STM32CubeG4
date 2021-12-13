/**
  @page UART_WakeUpFromStopUsingFIFO  wake up from STOP mode using UART FIFO level example

  @verbatim
  ******************************************************************************
  * @file    UART/UART_WakeUpFromStopUsingFIFO/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the UART_WakeUpFromStopUsingFIFO example.
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

This example shows how to use UART HAL API to wake up the MCU from STOP mode
using the UART FIFO level.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32G431RBTx Devices.

In the first part of the example, the UART is configured for reception with Rx FIFO threshold
set to one 1 QUARTER and the CPU enters into Stop Mode.
The UART FIFO threshold interrupt is enabled and, by default, the UART FIFO full is disabled.
The user has to send 2 bytes from the HyperTerminal. After 2 received bytes,
the UART FIFO will reach the quarter level which will generate an interrupt and wake up the CPU.

In the second part of the example, the UART is configured for reception with
FIFO threshold interrupt disabled and FIFO full enabled. then the CPU goes to stop mode.
The user has to send 8 bytes (size of the UART FIFO) from the HyperTerminal.
After 8 received bytes, the UART FIFO will be full, which will generate an interrupt and wake up the CPU.


Board: NUCLEO-G431RB RevC
Tx Pin: PA2
Rx Pin: PA3
Virtual COM Port using LPUART1: SB13, SB17, SB19, SB23: ON; SB12, SB18, SB20, SB22: OFF
   _________________________
  |           ______________|                       _______________
  |          |USART         |                      | HyperTerminal |
  |          |              |                      |               |
  |          |           TX |______________________|RX             |
  |          |              |                      |               |
  |          |              |     ST-Link Cable    |               |
  |          |           RX |______________________|TX             |
  |          |              |                      |               |
  |          |______________|                      |_______________|
  |                         |
  |                         |
  |                         |
  |                         |
  |_STM32_Board_____________|

LED2 is ON when MCU is not in STOP mode.
LED2 is blinking when there is an error occurrence.

The UART is configured as follows:
    - BaudRate = 9600 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

@note When the parity is enabled, the computed parity is inserted at the MSB
      position of the transmitted data.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@par Keywords

Connectivity, UART, Baud rate, RS-232, full-duplex, HyperTerminal,Transmission, Reception, Asynchronous,
STOP mode, FIFO.


@par Directory contents

  - UART/UART_WakeUpFromStopUsingFIFO/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - UART/UART_WakeUpFromStopUsingFIFO/Src/main.c                 Main program
  - UART/UART_WakeUpFromStopUsingFIFO/Src/system_stm32g4xx.c     STM32G4xx system clock configuration file
  - UART/UART_WakeUpFromStopUsingFIFO/Src/stm32g4xx_it.c         Interrupt handlers
  - UART/UART_WakeUpFromStopUsingFIFO/Src/stm32g4xx_hal_msp.c    HAL MSP module
  - UART/UART_WakeUpFromStopUsingFIFO/Inc/main.h                 Main program header file
  - UART/UART_WakeUpFromStopUsingFIFO/Inc/stm32g4xx_hal_conf.h   HAL Configuration file
  - UART/UART_WakeUpFromStopUsingFIFO/Inc/stm32g4xx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.

  - This example has been tested with NUCLEO-G431RB RevC board and can be
    easily tailored to any other supported device and development board.

  - STM32G431RBTx Set-up
      Connect a USB cable between the ST-Link USB connector
      and PC to display data on the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - Flow control: None

@par How to use it?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
