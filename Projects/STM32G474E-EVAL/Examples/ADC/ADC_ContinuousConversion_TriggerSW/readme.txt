/**
  @page ADC_ContinuousConversion_TriggerSW ADC example

  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_ContinuousConversion_TriggerSW/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_ContinuousConversion_TriggerSW example.
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
This example provides a short description of how to use the ADC peripheral to
perform conversions in continuous mode.

Example configuration:
ADC is configured to convert a single channel, in continuous conversion mode,
from SW trigger.

Example execution:
The ADC launch conversions in continuous mode on the selected channel.
Then waits for the first conversion completion before reading the conversion results.
With the continuous conversion mode and the data overwrite functionality enabled,
ADC is continuously converting data and updating the ADC data register with new values.

For debug: variables to monitor with debugger watch window:
 - "uhADCxConvertedData": ADC group regular conversion data
 - "uhADCxConvertedData_Voltage_mVolt": ADC conversion data computation to physical values
 
Connection needed:
None.
Note: Voltage on analog input pin is provided by potentiometer on board,
      to perform a ADC conversion on a determined voltage level.

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PC0 (pin 46 on connector CN5)

Board settings:
 - ADC is configured to convert ADC_CHANNEL_6 (pin 46 on connector CN5).
 - The voltage input on ADC channel is provided by the on-board potentiometer (RV2). Turn RV2 to vary the ADC input voltage and observe behavior.
 - Connect jumper JP5 on 2-3 position (LDR)
 - Connect a wire between JP5 pin 1 and PC0

To observe voltage level applied on ADC channel through GPIO, connect a voltmeter on
pin PC0 (pin 46 on connector CN5).

STM32G474E-EVAL Rev B board LED is be used to monitor the program execution status:
 - Normal operation: LED1 is toggling after each data read
 - Error: In case of other errors, LED1 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, ADC, Analog to Digital, continuous mode, Continuous conversion, trigger software

@par Directory contents 

  - ADC/ADC_ContinuousConversion_TriggerSW/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - ADC/ADC_ContinuousConversion_TriggerSW/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - ADC/ADC_ContinuousConversion_TriggerSW/Inc/main.h                  Header for main.c module  
  - ADC/ADC_ContinuousConversion_TriggerSW/Src/stm32g4xx_it.c          Interrupt handlers
  - ADC/ADC_ContinuousConversion_TriggerSW/Src/main.c                  Main program
  - ADC/ADC_ContinuousConversion_TriggerSW/Src/system_stm32g4xx.c      STM32G4xx system source file


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
