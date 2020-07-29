/**
  @page CORDIC_CosSin CORDIC Cosine and Sine calculation example

  @verbatim
  ******************************************************************************
  * @file    Examples_LL/CORDIC/CORDIC_CosSin/readme.txt
  * @author  MCD Application Team
  * @brief   Description of CORDIC_CosSin example.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to use the CORDIC peripheral to calculate cosine and sine.

This example is based on the STM32G4xx CORDIC LL API.
The peripheral initialization and control is done using LL unitary service
functions for optimization purpose (performance and size).

In this example, the CORDIC peripheral is configured in cosine function, q1.31
format for both input and output data, and with 6 cycles of precision.
The input data provided to CORDIC processor are angles in radians
divided by PI, in q1.31 format. The output data are cosine and sine in q1.31
format. For 6 cycles of precision, the maximal expected residual error of the
calculated sines is 2^-19.

The calculated cosine and sine are stored in cosOutput and sinOutput variables.
The residual error of calculation results is verified, by comparing to
reference values.

STM32 board LED2 is used to monitor the example status:
  - LED2 is ON when correct CORDIC cosine and sine results are calculated.
  - LED2 is blinking (2 second period) when exceeding residual error
    on CORDIC results is detected or when there is an initialization error.

@par Keywords

CORDIC, Cosine, Sine. 

@par Directory contents

  - CORDIC/CORDIC_CosSin/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - CORDIC/CORDIC_CosSin/Inc/main.h                  Header for main.c module
  - CORDIC/CORDIC_CosSin/Inc/stm32_assert.h          Template file to include assert_failed function
  - CORDIC/CORDIC_CosSin/Src/stm32g4xx_it.c          Interrupt handlers
  - CORDIC/CORDIC_CosSin/Src/main.c                  Main program
  - CORDIC/CORDIC_CosSin/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G491RETx devices.

  - This example has been tested with STMicroelectronics NUCLEO-G491RE RevA
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
