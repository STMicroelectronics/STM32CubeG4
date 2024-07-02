/**
  @page CRC_Bytes_Stream_7bit_CRC  Bytes Buffers 7-bit CRC Computation Example

  @verbatim
  ******************************************************************************
  * @file    CRC/CRC_Bytes_Stream_7bit_CRC/readme.txt
  * @author  MCD Application Team
  * @brief   7-bit long CRC computation from bytes (8-bit data) buffers.
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

How to configure the CRC using the HAL API. The CRC (cyclic
redundancy check) calculation unit computes 7-bit CRC codes derived from buffers
of 8-bit data (bytes). The user-defined generating polynomial is manually set
to 0x65, that is, X^7 + X^6 + X^5 + X^2 + 1, as used in the Train Communication
Network, IEC 60870-5[17].

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK).

The CRC peripheral configuration is ensured by HAL_CRC_Init() function.
The latter is calling HAL_CRC_MspInit() function which core is implementing
the configuration of the needed CRC resources according to the used hardware (CLOCK).
You can update HAL_CRC_Init() input parameters to change the CRC configuration.

In this example, the user-defined generating polynomial is configured by
HAL_CRC_Init(). At the same time, it is set that neither input or output data
must be reversed, the default init value is used and it is specified that input
data type is byte.

First, a 5-byte long buffer is processed to yield a first CRC.

Next, a second CRC is computed from a 17-byte long buffer. For the latter,
the CRC calculator is not re-initialized and instead the previously computed CRC
is used as initial value.

Then, a third CRC is computed from a 1-byte long buffer. Again, the CRC calculator
is not re-initialized, the previously computed CRC is used as initial value.

Finally, a fourth CRC is computed from a 2-byte long buffer. This time, the CRC
calculator is re-initialized with the IP default value that is 0x7F for a 7-bit CRC.
This is done with a call to HAL_CRC_Calculate() instead of HAL_CRC_Accumulate().

Each time, the calculated CRC code is stored in uwCRCValue variable.
Once calculated, the CRC value (uwCRCValue) is compared to the CRC expected value (uwExpectedCRCValue_1, 2, 3 or 4).

STM32 board LEDs are used to monitor the example status:
  - LED1 (GREEN) is ON when the correct CRC value is calculated
  - LED3 (RED) is ON when there is an error in initialization or if an incorrect CRC value is calculated.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
 to have correct HAL operation.

@par Keywords

Security, CRC, CRC Polynomial, IEC 60870-5

@par Directory contents

  - CRC/CRC_Bytes_Stream_7bit_CRC/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - CRC/CRC_Bytes_Stream_7bit_CRC/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - CRC/CRC_Bytes_Stream_7bit_CRC/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - CRC/CRC_Bytes_Stream_7bit_CRC/Inc/main.h                  Header for main.c module
  - CRC/CRC_Bytes_Stream_7bit_CRC/Src/stm32g4xx_it.c          Interrupt handlers
  - CRC/CRC_Bytes_Stream_7bit_CRC/Src/main.c                  Main program
  - CRC/CRC_Bytes_Stream_7bit_CRC/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - CRC/CRC_Bytes_Stream_7bit_CRC/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.
  - This example has been tested with STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */

