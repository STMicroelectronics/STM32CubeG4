/**
  @page CORDIC_SinCos_DMA_Perf CORDIC Sine and Cosine calculation on array in DMA mode example

  @verbatim
  ******************************************************************************
  * @file    CORDIC/CORDIC_SinCos_DMA_Perf/readme.txt
  * @author  MCD Application Team
  * @brief   Sine and Cosine calculation on array in DMA mode example.
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

How to use the CORDIC peripheral to calculate sines and cosines array in DMA mode.

This example is based on the STM32G4xx CORDIC HAL API.

In this example, the CORDIC peripheral is configured in sine function, q1.31
format for both input and output data, and with 6 cycles of precision.
The input data provided to CORDIC peripheral are angles in radians
divided by PI, in q1.31 format. The output data are sines and cosines in q1.31
format. For 6 cycles of precision, the maximal expected residual error of the
calculated sines is 2^-19.

DMA is used to transfer input data from memory to the CORDIC peripheral and
output data from CORDIC peripheral to memory, so that CPU is offloaded.

At start of execution, the calculation of sines and cosines are performed
in loop using CORDIC peripheral. When User push-button is pushed, the same
calculation is performed in loop using arm math software library. Each time
the User push-button is pushed again the calculation toggles between CORDIC
peripheral and arm math software library usage.

LED2 blinks at speed of calculation loops.
This shows the performance gain of CORDIC peripheral usage (fast blink)
compared to arm math software library usage (slow blink) in this use case.

The calculated sines are stored in aResults[] array.
This array contains alternatively the calculated sine and the calculated cosine
of each input angle provided.
The residual error of calculation results is verified, by comparing to
reference values in aRefSin[] and aRefCos[] obtained from double precision
floating point calculation.
In case of exceeding residual error, the LED2 is turned off and stops blinking.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The example needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

CORDIC, Sine, Cosine, DMA mode.


@par Directory contents

  - CORDIC/CORDIC_SinCos_DMA_Perf/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - CORDIC/CORDIC_Sinus_DMA/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - CORDIC/CORDIC_Sinus_DMA/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - CORDIC/CORDIC_Sinus_DMA/Inc/main.h                  Header for main.c module
  - CORDIC/CORDIC_Sinus_DMA/Src/stm32g4xx_it.c          Interrupt handlers
  - CORDIC/CORDIC_Sinus_DMA/Src/main.c                  Main program
  - CORDIC/CORDIC_Sinus_DMA/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - CORDIC/CORDIC_Sinus_DMA/Src/system_stm32g4xx.c      STM32G4xx system source file


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
