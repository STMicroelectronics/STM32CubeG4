/**
  @page FMAC_IIR_Polling in polling mode example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/FMAC/FMAC_IIR_Polling/readme.txt
  * @author  MCD Application Team
  * @brief   Description of FMAC_IIR_filtering in polling mode.
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

How to use the FMAC peripheral to achieve IIR filtering in polling mode.

This example is based on the stm32g4xx FMAC LL API.
The peripheral initialization and control is done using LL unitary service
functions for optimization purpose (performance and size).

In this example, the FMAC peripheral is configured to achieve IIR filtering.
Four possible filters are available
- HIGHPASS with order 1
- LOWPASS with order 1
- BANDPASS with order 2
- BANDSTOP with order 2
User is expected to set the desired setting in IAR command line, for example set
ORDER=1
HIGHPASS
in the command line available in:
Options -> C/C++ Compiler -> Preprocessor -> 'Defined symbols: (one per line)' box.
This setting selects the filter coefficients and number in main.h.

aInputValues[] array is filtered and output data samples are stored in aCalculatedFilteredData[].
Input data samples are written and output data samples are read out in polling mode.

STM32 board LED2 is used to monitor the example status:
  - LED2 is blinking (1 second period) when there is a saturation, an underflow or an overflow error.
  - LED2 is on otherwise (no error detected)

@par Keywords

FMAC, IIR, HIGHPASS, LOWPASS, BANDPASS, BANDSTOP.

@par Directory contents

  - FMAC/FMAC_IIR_Polling/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - FMAC/FMAC_IIR_Polling/Inc/main.h                  Header for main.c module
  - FMAC/FMAC_IIR_Polling/Inc/stm32_assert.h          Template file to include assert_failed function
  - FMAC/FMAC_IIR_Polling/Src/stm32g4xx_it.c          Interrupt handlers
  - FMAC/FMAC_IIR_Polling/Src/main.c                  Main program
  - FMAC/FMAC_IIR_Polling/Src/system_stm32g4xx.c      stm32g4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G431RBTx devices.

  - This example has been tested with STMicroelectronics NUCLEO-G431RB RevC
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
