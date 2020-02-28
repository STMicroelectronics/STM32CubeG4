/**
  @page UART_Printf UART Printf example
  
  @verbatim
  ******************************************************************************
  * @file    UART/UART_Printf/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the UART Printf example.
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description

Re-routing of the C library printf function to the UART.
The UART outputs a message on the HyperTerminal.

Board: STM32G474E-EVAL
Tx Pin: PA.09: pin 8 on CN5 (available through ST-Link)
Rx Pin: PA.10: pin 4 on CN5 (available through ST-Link)
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

LED3 turns ON when there is an error.

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

Connectivity, UART, Printf, Baud rate, RS-232, HyperTerminal, full-duplex, Transmission,
Reception, Asynchronous, interrupt

@par Directory contents

  - UART/UART_Printf/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - UART/UART_Printf/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - UART/UART_Printf/Inc/stm32g4xx_it.h          IT interrupt handlers header file
  - UART/UART_Printf/Inc/main.h                  Header for main.c module
  - UART/UART_Printf/Src/stm32g4xx_it.c          Interrupt handlers
  - UART/UART_Printf/Src/main.c                  Main program
  - UART/UART_Printf/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - UART/UART_Printf/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.
    
  - This example has been tested with STM32G474E-EVAL board and can be
    easily tailored to any other supported device and development board.   

  - STM32G474E-EVAL Set-up
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


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
