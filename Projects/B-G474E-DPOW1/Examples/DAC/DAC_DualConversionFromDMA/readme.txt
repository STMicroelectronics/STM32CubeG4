/**
  @page DAC_DualConversionFromDMA DAC example

  @verbatim
  ******************************************************************************
  * @file    Examples/DAC/DAC_DualConversionFromDMA/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DAC_DualConversionFromDMA example.
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
Use DAC dual channel mode with DMA to generate signal on both DAC channels at the same time.

Example configuration:
DAC conversions are made upon timer trigger and data feed from DMA.
DAC channel 1 is used to trig DMA feeding. Both DAC channels are triggered by the
same timer.

Example execution:
From the start, the timer is configured to trig the DAC channels at about 1kHz.
DMA is feeding both channel at the time.
Channel 1 pattern is ~0V/~1.65V/~3.3V/~1.65V
Channel 2 pattern is ~1.65V/~3.3V/~1.65V/~0V

Connection needed:
Oscilloscope probe need to be connected to PA4 (pin 26 connector CN8)
and PA5 (pin 25 connector CN8) to observe wave generation effects.

Other peripherals used:
  1 GPIO for LED
  2 GPIOs for analog output: PA4 (pin 26 connector CN8) and PA5 (pin 25 connector CN8)
  
Board settings:



B-G474E-DPOW1 board LED is be used to monitor the program execution status:
 - Normal operation: LED4 is turned-on
 - Error: In case of error, LED4 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, DAC, Digital to Analog, Single conversion, Timer trigger, DMA, OPAMP

@par Directory contents 

  - DAC/DAC_DualConversionFromDMA/Inc/b_g474e_dpow1_conf.h     BSP configuration file
  - DAC/DAC_DualConversionFromDMA/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - DAC/DAC_DualConversionFromDMA/Inc/main.h                        Header for main.c module  
  - DAC/DAC_DualConversionFromDMA/Src/stm32g4xx_it.c          Interrupt handlers
  - DAC/DAC_DualConversionFromDMA/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - DAC/DAC_DualConversionFromDMA/Src/main.c                        Main program
  - DAC/DAC_DualConversionFromDMA/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xx devices.
    
  - This example has been tested with B-G474E-DPOW1 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
