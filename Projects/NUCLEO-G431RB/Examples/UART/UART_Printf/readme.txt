/**
  @page UART_Printf UART Printf example
  
  @verbatim
  ******************************************************************************
  * @file    UART/UART_Printf/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the UART Printf example.
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

Re-routing of the C library printf function to the UART.
The UART outputs a message on the HyperTerminal.

Board: NUCLEO-G431RB
Tx Pin: PA.02: available through ST-Link
Rx Pin: PA.03: available through ST-Link
   _________________________ 
  |           ______________|                       _______________
  |          |USART         |                      | HyperTerminal |
  |          |              |                      |               |
  |          |           TX |______________________|RX             |
  |          |              |                      |               |
  |          |              |    ST-Link Cable     |               |
  |          |              |                      |               |
  |          |           RX |______________________|TX             |
  |          |              |                      |               |
  |          |______________|                      |_______________|
  |                         |
  |                         |
  |                         |
  |                         |
  |_STM32_Board_____________|                      

LED2 toggles when there is an error.

The USART is configured as follows:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

@note When the parity is enabled, the computed parity is inserted at the MSB
position of the transmitted data.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Connectivity, UART, Printf, Baud rate, RS-232, HyperTerminal, full-duplex, HyperTerminal, 
Transmission, Reception, Asynchronous, interrupt

@par Directory contents

  - UART/UART_Printf/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - UART/UART_Printf/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - UART/UART_Printf/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - UART/UART_Printf/Inc/main.h                  Header for main.c module
  - UART/UART_Printf/Src/stm32g4xx_it.c          Interrupt handlers
  - UART/UART_Printf/Src/main.c                  Main program
  - UART/UART_Printf/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - UART/UART_Printf/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.
    
  - This example has been tested with NUCLEO-G431RB board and can be
    easily tailored to any other supported device and development board.   

  - NUCLEO-G431RB Set-up
    Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.
      A virtual COM port will then appear in the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 115200 baud
    - Flow control: None 

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 */
