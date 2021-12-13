/**
  @page LPUART_TwoBoards_ComIT LPUART Two Boards Communication IT example

  @verbatim
  ******************************************************************************
  * @file    UART/LPUART_TwoBoards_ComIT/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the LPUART_TwoBoards_ComIT example.
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

LPUART transmission (transmit/receive) in Interrupt mode
between two boards.

Board: NUCLEO-G431RB RevC (embeds a STM32G431RBT6 device)
Tx Pin: PC.01 (pin 36 on CN7)
Rx Pin: PC.00 (pin 38 on CN7)
   _________________________                       _________________________
  |           ______________|                     |______________           |
  |          |LPUART        |                     |        LPUART|          |
  |          |              |                     |              |          |
  |          |           TX |_____________________| RX           |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |           RX |_____________________| TX           |          |
  |          |              |                     |              |          |
  |          |______________|                     |______________|          |
  |                         |                     |                         |
  |                         |                     |                         |
  |                      GND|_____________________|GND                      |
  |_STM32_Board 1___________|                     |_STM32_Board 2___________|



Two identical boards are connected as shown on the picture above.
Board 1: transmitting then receiving board
Board 2: receiving then transmitting board

NB: After plugging boards already loaded with the example, it is better to either:
      - reset them using the RESET button;
      - start again the example using your preferred IDE.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

The user presses the User push-button on board 1.
Then data are transmitted through LPUART1 in IT mode. 2nd board has configured LPUART1
to receive data in IT mode. Once data received is completed, 2nd board also
transmit same characters on LPUART1 which will be received by 1st board.
Both boards compare transmitted data to received ones. If they are same,
LED2 is turned on, else LED2 is turned off.

Warning: As both boards do not behave same way, "TRANSMITTER_BOARD" switch
compilation exists to determine either software is for 1st transmitter board or
2nd receiver (then transmitter) board. In other words, 1st board has to be
flashed with software compiled with switch enable, 2nd board has to be flashed
with software compiled with switch disable.

LED2 can be used to monitor the transfer status:
 - LED2 turns ON on master board waiting User push-button to be pressed.
 - LED2 turns OFF on master board waiting the transmission process complete.
 - LED2 turns ON when the transmission/reception process is correct.
 - If there is an initialization or transfer error, LED2 is slowly blinking (1 sec. period).

The LPUART is configured as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - None parity
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

Connectivity, LPUART, Baud rate, RS-232, Full-duplex, DMA, Parity, Stop bit, Transmission, Reception, Low power.

@par Directory contents

  - UART/LPUART_TwoBoards_ComIT/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - UART/LPUART_TwoBoards_ComIT/Src/main.c                 Main program
  - UART/LPUART_TwoBoards_ComIT/Src/system_stm32g4xx.c     STM32G4xx system clock configuration file
  - UART/LPUART_TwoBoards_ComIT/Src/stm32g4xx_it.c         Interrupt handlers
  - UART/LPUART_TwoBoards_ComIT/Src/stm32g4xx_hal_msp.c    HAL MSP module
  - UART/LPUART_TwoBoards_ComIT/Inc/main.h                 Main program header file
  - UART/LPUART_TwoBoards_ComIT/Inc/stm32g4xx_hal_conf.h   HAL Configuration file
  - UART/LPUART_TwoBoards_ComIT/Inc/stm32g4xx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.

  - This example has been tested with NUCLEO-G431RB RevC board and can be
    easily tailored to any other supported device and development board.

  - STM32G431RBTx Set-up
    - Connect a wire between 1st board PC.01 (pin 36 on CN7) pin (LPUART Tx) and 2nd board PC.00 (pin 38 on CN7) pin (LPUART Rx).
    - Connect a wire between 1st board PC.00 (pin 38 on CN7) pin (LPUART Rx) and 2nd board PC.01 (pin 36 on CN7) pin (LPUART Tx).
    - Connect 1st board GND to 2nd Board GND.

@par How to use it?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
