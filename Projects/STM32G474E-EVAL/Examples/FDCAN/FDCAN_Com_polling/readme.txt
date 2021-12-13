/**
  @page FDCAN_Com_polling FDCAN Communication polling example

  @verbatim
  ******************************************************************************
  * @file    FDCAN/FDCAN_Com_polling/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FDCAN_Com_polling example.
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

How to achieve Polling Process Communication between two FDCAN units.

The sent frames are used to control LEDs by pressing User push-button.

The CAN serial communication link is a bus to which a number of units may be
connected. This number has no theoretical limit. Practically the total number
of units will be limited by delay times and/or electrical loads on the bus line.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 80 MHz.

Then, on each of the two FDCAN instances of the board:
  - FDCAN module is configured to operate in normal mode,
    with Nominal Bit Rate of 1 MBit/s and Data Bit Rate of 8 MBit/s.
  - FDCAN module is configured to receive messages with pre-defined ID to its Rx FIFO 0.
  - FDCAN module is configured with Tx delay compensation, required for Bit Rate
    Switching mode.
  - FDCAN module is started.
  - The main program is polling for message to be received in RX FIFO 0 of each FDCAN instance,
    using HAL_FDCAN_GetRxFifoFillLevel().

When User push-button is pressed:
  - LED1 and LED4 are turned OFF
  - Alternatively, one FDCAN module sends a message to the other FDCAN instance,
    with Bit Rate Switching.

On the other FDCAN instance, after receiving the message:
 - received payload is compared to expected data
 - if the result is OK, and message is received by first FDCAN instance, LED1 is turned ON
 - if the result is OK, and message is received by second FDCAN instance, LED4 is turned ON

If at any time of the process an error is encountered LED3 is turned ON.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

FDCAN, Polling, FD, BRS, TxDelay

@par Directory contents

  - FDCAN/FDCAN_Com_polling/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32g4xx_it.h          Header for stm32g4xx_it.c
  - FDCAN/FDCAN_Loopback/Inc/main.h                  Header for main.c module
  - FDCAN/FDCAN_Loopback/Src/stm32g4xx_it.c          Interrupt handlers
  - FDCAN/FDCAN_Loopback/Src/main.c                  Main program
  - FDCAN/FDCAN_Loopback/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - FDCAN/FDCAN_Loopback/Src/system_stm32g4xx.c      stm32g4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.

  - This example has been tested with a STM32G474E-EVAL Rev B board and can be
    easily tailored to any other supported device and development board.

  - STM32G474E-EVAL set-up:
    - Jumper JP1 => fitted
    - Jumper JP2 => fitted
    - Jumper JP4 => fitted
    - Connect the 2 points CN12 CAN connector (CAN-H and CAN-L) of FDCAN1
      instance, to the 2 points CN10 CAN connectors (resp. CAN-H and CAN-L) of
      FDCAN2 instance.

@par How to use it ?

In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example

 */
