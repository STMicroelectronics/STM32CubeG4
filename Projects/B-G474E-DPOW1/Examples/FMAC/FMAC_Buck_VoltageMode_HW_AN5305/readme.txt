/**
  @page FMAC_Buck_VoltageMode_HW_AN5305 FMAC 3p3z IIR compensator example

  @verbatim
  ******************************************************************************
  * @file    FMAC/FMAC_Buck_VoltageMode_HW_AN5305/readme.txt
  * @author  MCD Application Team
  * @brief   Description of FMAC_Buck_VoltageMode_HW_AN5305 example.
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

How to use the FMAC peripheral to implement a 3p3z controller.

This example implements a voltage mode controller for a synchronous buck
converter. The compensator is a 3-pole 3-zero (3p3z) IIR filter.

The FMAC peripheral is configured to perform an IIR filter with
4 feed-forward taps, 3 feedback taps and a gain equal to 5. The input
and output thresholds are set to 1. The clipping feature is enabled.

This local memory of the FMAC peripheral is divided into three buffers:
 -   0 to   6: coefficient buffer;
 -   7 to  11: input buffer;
 -  12 to  16: output buffer;
 -  17 to 255: unused.

The coefficient buffer contains the concatenation of:
 - the vector of feed-forward taps, named B (here, size N = 4 elements);
 - the vector of feedback taps, named A (size comprised in [1; N-1];
   here, size M = 3 elements).
This buffer is initialized during the configuration step in polling mode.

The size of the input buffer is the sum of:
 - the size of the coefficient vector B (N = 4 elements);
 - the size of the additional space D1 needed for throughput improvement
   (minimum: 0; here, D1 = 1 elements).

The size of the output buffer is the sum of:
 - the size of the coefficient A buffer (M = 3 elements);
 - the size of the additional space D2 needed for a better throughput
   (minimum: 1; here, D2 = 2 elements).
The first M elements of the output buffer are set to 0 during the preload step
in polling mode.

DMA mode is used to transfer input data from ADC to the FMAC peripheral.
IT mode is used to transfer output data from FMAC peripheral to
HRTIM (via CPU).

VIN (CN4.2) needs to be connected to 3.3 V. This can be provided by an external
power source (don't forget to connect GND in this case) or, for instance, by 3V3_O (CN8.1).

A press on the Joystick Right push-button increases the activated resistors load.
A press on the Joystick LEFT push-button decreases the activated resistors load.
The green LED4 and the orange LED3 indicate the status
of the activated resistors load:
  - LED4 OFF and LED3 OFF: minimum;
  - LED4  ON and LED3 OFF: half-load;
  - LED4  ON and LED3  ON: maximum.

The blue LED2 and the red LED5 indicate the example status:
  - LED2 is blinking (1 second period) if the example is running
    and if the compilation switch CHECK_CPU_IDLE_LOOP is disabled in main.h.
  - LED5 is blinking (1 second period) if there is an initialization
    or configuration error.

As stated in the application note AN5305, using an oscilloscope:
  - the high side switch drive can be monitored on BUCKBOOST_P1_DRIVE (CN8.15 - PB12).
  - the low side switch drive can be monitored on BUCKBOOST_N1_DRIVE (CN8.14 - PB13).
  - the buck converter output can be monitored on VOUT (CN7.1).
    It slightly decreases when the activated resistors load increases.
  - the CPU idle loop can be monitored on the GPIO GPO1 (CN9.22 - PB9)
    if the compilation switch CHECK_CPU_IDLE_LOOP is enabled in main.h.
    The blue LED2 doesn't blink when the feature is enabled.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@par Keywords

FMAC, IIR, Filter, Voltage mode controller, Polling mode,IT mode, DMA mode, HRTIM, AN5305, Oscilloscope.

@par Directory contents

  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Src/main.c                  Main program
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Src/system_stm32g4xx.c      STM32G4xx system clock configuration file
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Src/stm32g4xx_it.c          Interrupt handlers
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Inc/main.h                  Main program header file
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Inc/stm32g4xx_hal_conf.h    HAL Configuration file
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - FMAC/FMAC_Buck_VoltageMode_HW_AN5305/Inc/b_g474e_dpow1_conf.h BSP configuration file


@par Hardware and Software environment

  - This example runs on STM32G474RETx devices.

  - This example has been tested with B-G474E-DPOW1 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
