/**
  @page DAC_DualConversion DAC example

  @verbatim
  ******************************************************************************
  * @file    Examples/DAC/DAC_DualConversion/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DAC_DualConversion example.
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
Use DAC dual channel mode to generate signal on both DAC channels at the same time.

Example configuration:
DAC conversions are made as soon as data are written in register through call of
HAL_DACEx_DualSetValue API.

Example execution:
At startup, DAC channels are converted as follow:
- Channel 1: ~0V
- Channel 2: ~1.65V

Upon each press on User push-button, the data to convert for each DAC channels are
changed changes to next one in list:
Configuration #0:
- Channel 1: ~0V
- Channel 2: ~1.65V
Configuration #1:
- Channel 1: ~1.65V
- Channel 2: ~3.3V
Configuration #2:
- Channel 1: ~3.3V
- Channel 2: ~1.65V
Configuration #3:
- Channel 1: ~1.65V
- Channel 2: ~0V

Note: when configuration #4 is selected, a press on User push-button returns to initial
offset configuration. Subsequent presses will allow to go through above configuration again.
                  
For debug: variable to monitor with debugger watch window:
 - "waveIndex": waveIndex is equal to current configuration number

Connection needed:
Oscilloscope probe need to be connected to PA4 (pin 41 connector CN6)
and PA5 (pin 40 connector CN6) to observe wave generation effects.

Other peripherals used:
  1 GPIO for LED
  2 GPIOs for analog output: PA4 (pin 41 connector CN6) and PA5 (pin 40 connector CN6)
  1 GPIO for push button
  
Board settings:



STM32G474E-EVAL1 Rev B board LED is be used to monitor the program execution status:
 - Normal operation: LED1 is turned-on
 - Error: In case of error, LED1 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, DAC, Digital to Analog, Single conversion, Timer trigger, DMA, OPAMP

@par Directory contents 

  - DAC/DAC_DualConversion/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - DAC/DAC_DualConversion/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - DAC/DAC_DualConversion/Inc/main.h                        Header for main.c module  
  - DAC/DAC_DualConversion/Src/stm32g4xx_it.c          Interrupt handlers
  - DAC/DAC_DualConversion/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - DAC/DAC_DualConversion/Src/main.c                        Main program
  - DAC/DAC_DualConversion/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xx devices.
    
  - This example has been tested with STM32G474E-EVAL1 Rev B board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
