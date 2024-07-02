/**
  @page FDCAN_Loopback FDCAN Loopback example

  @verbatim
  ******************************************************************************
  * @file    FDCAN/FDCAN_Loopback/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FDCAN_Loopback example.
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

How to configure the FDCAN to operate in loopback mode.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 80 MHz.

Then:
  - FDCAN module is configured to operate in external loopback mode,
    with Nominal Bit Rate of 1 MBit/s and Data Bit Rate of 8 MBit/s.
  - FDCAN module is configured to receive:
     - messages with pre-defined standard ID to Rx FIFO 0
     - messages with pre-defined extended ID to Rx FIFO 1
  - FDCAN module is started.
  - The following messages are then sent:
     - one standard ID message matching Rx FIFO 0 filter
     - two extended ID messages matching Rx FIFO 1 filter

Main program checks that the three messages are received as expected
If the result is OK, LED1 start blinking
If the messages are not correctly received, or if at any time of the process an error is encountered,
LED3 is turned ON.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

FDCAN, Loopback, Polling

@par Directory contents

  - FDCAN/FDCAN_Loopback/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32g4xx_it.h          Header for stm32g4xx_it.c
  - FDCAN/FDCAN_Loopback/Inc/main.h                  Header for main.c module
  - FDCAN/FDCAN_Loopback/Src/stm32g4xx_it.c          Interrupt handlers
  - FDCAN/FDCAN_Loopback/Src/main.c                  Main program
  - FDCAN/FDCAN_Loopback/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - FDCAN/FDCAN_Loopback/Src/system_stm32g4xx.c      stm32g4xx system source file

@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.

  - This example has been tested with an STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.

  - STM32G474E-EVAL1 set-up
    - Jumper JP1 => fitted
    - Jumper JP4 => fitted

@par How to use it ?

In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example

 */
