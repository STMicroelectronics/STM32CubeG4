/**
  @page DAC_DMADoubleDataMode DAC example

  @verbatim
  ******************************************************************************
  * @file    Examples/DAC/DAC_DMADoubleDataMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DAC_DMADoubleDataMode example.
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
Use DAC DMA double data mode to save AHB bandwidth and to be able to output 2 different
250kHz sine wave sampled at 15MSps by 2 different DAC converters.

Example configuration:
Two sine waves are generated upon the same timer trigger and conversion data are given by 2 DMA channels,
operating in circular mode.

Example execution:
From the start, DMA double data mode is not enabled. User may observe 2 signals which are not
at the expected frequency (250kHz), nor synchronized.

Upon each press on User push-button, the DMA double data mode is toggled (enabled or disabled).
When DMA double data mode is enabled, user may observe 2 sine waves at the expected frequency (250kHz) and synchronized.

For debug: variable to monitor with debugger watch window:
 - "vDmaDoubleDataMode": DAC DMA double data mode configuration

Connection needed:
Oscilloscope probe need to be connected to PB1 (pin 33 connector CN6)
and PA8 (pin 2 connector CN5) to observe waves generation effects.

Other peripherals used:
  1 GPIO for LED
  2 GPIO for analog output: PB1 (pin 33 connector CN6) and PA8  (pin 2 connector CN5)
  1 GPIO for push button
  TIMER
  DMA

Board settings:



STM32G474E-EVAL Rev B board LED is be used to monitor the program execution status:
 - Normal operation: LED1 is turned-on
 - Error: In case of error, LED1 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, DAC, Digital to Analog, Single conversion, Timer trigger, DMA, OPAMP

@par Directory contents 

  - DAC/DAC_DMADoubleDataMode/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - DAC/DAC_DMADoubleDataMode/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - DAC/DAC_DMADoubleDataMode/Inc/main.h                        Header for main.c module  
  - DAC/DAC_DMADoubleDataMode/Src/stm32g4xx_it.c          Interrupt handlers
  - DAC/DAC_DMADoubleDataMode/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - DAC/DAC_DMADoubleDataMode/Src/main.c                        Main program
  - DAC/DAC_DMADoubleDataMode/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xx devices.
    
  - This example has been tested with STM32G474E-EVAL Rev B board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
