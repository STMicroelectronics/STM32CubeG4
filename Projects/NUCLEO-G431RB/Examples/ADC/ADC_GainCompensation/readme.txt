/**
  @page ADC_GainCompensation ADC example

  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_GainCompensation/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_GainCompensation example.
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
Use ADC Gain compensation feature to get directly voltage in mVolt from conversion
without need of data post computing.

Example configuration:
The channel is converted upon a timer trigger and result of conversions are stored
into a variable by DMA, operating in circular mode.

Example execution:
From the start, the ADC converts the selected channel continuously, DMA transfers
conversion data to a variable:
    uhADCxConvertedData_Voltage_mVolt

For debug: variable to monitor with debugger watch window:
 - "uhADCxConvertedData_Voltage_mVolt": ADC converted data in mVolt

Connection needed:
None.
Note: Optionally, a voltage can be supplied to the analog input pin (cf pin below),
      between 0V and Vdda=3.3V, to perform a ADC conversion on a determined
      voltage level.
      Otherwise, this pin can be let floating (in this case ADC conversion data
      will be undetermined).

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PC0 (pin 38 on connector CN7)
  TIMER
  DMA

Board settings:
 - The voltage input on the ADC external channel can be provided by an external source
   connected to pin 38 on connector CN7.



NUCLEO-G431RB RevC board LED is be used to monitor the program execution status:
 - Normal operation: LED2 is turned-on
 - Error: In case of error, LED2 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, ADC, Analog to Digital, Single conversion, Gain compensation, Timer trigger

@par Directory contents 

  - ADC/ADC_GainCompensation/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - ADC/ADC_GainCompensation/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - ADC/ADC_GainCompensation/Inc/main.h                        Header for main.c module  
  - ADC/ADC_GainCompensation/Src/stm32g4xx_it.c          Interrupt handlers
  - ADC/ADC_GainCompensation/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - ADC/ADC_GainCompensation/Src/main.c                        Main program
  - ADC/ADC_GainCompensation/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474xx devices.
    
  - This example has been tested with NUCLEO-G431RB RevC board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
