/**
  @page ADC_GroupsRegularInjected ADC example

  @verbatim
  ******************************************************************************
  * @file    Examples/ADC/ADC_GroupsRegularInjected/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the ADC_GroupsRegularInjected example.
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
Use ADC to perform conversions using the two ADC groups: regular group
for ADC conversion on main stream and injected group for ADC conversions
limited on specific events (conversions injected within main conversions
stream).

Example configuration:
ADC is configured to convert a regular channel, in single conversion mode,
from HW trigger: timer peripheral. And an injected channel, also in single
conversion mode, but from SW trigger.
DMA is configured to transfer reular channel conversion data in an array,
in circular mode.
A timer is configured in time base and to generate TRGO events for regular
channel conversions.

Example execution:
From the start, the ADC converts the selected regular channel at each trig from timer.
DMA transfers conversion data to the array, DMA transfer complete interruption occurs.
Results array is updated indefinitely (DMA in circular mode).
During this regular channel conversions, the injected channel conversion is launched by
software.
LED1 is turned on or off depending on whether injected channel conversion data is greater
or not than regular channel conversions average.

For debug: variables to monitor with debugger watch window:
 - "aADCxConvertedData": ADC group regular conversion data (array of data)
 - "uhADCxConvertedData_Injected": ADC injected group conversion data

Connection needed:
None.
Note: Voltage on analog input pin is provided by potentiometer on board,
      to perform a ADC conversion on a determined voltage level.

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog input: PC0 (pin 46 on connector CN5)
  DMA
  Timer

Board settings:
 - ADC is configured to convert ADC_CHANNEL_6 (pin 46 on connector CN5).
 - The voltage input on ADC channel is provided by the on-board potentiometer (RV2). Turn RV2 to vary the ADC input voltage and observe behavior.
 - Connect jumper JP5 on 2-3 position (LDR)
 - Connect a wire between JP5 pin 1 and PC0

To observe voltage level applied on ADC channel through GPIO, connect a voltmeter on
pin PC0 (pin 46 on connector CN5).

STM32G474E-EVAL Rev B board LED is be used to monitor the program execution status:
 - Normal operation: LED1 is turned-on/off in function of ADC conversion
   result.
    - "On" if injected channel conversion data is greater than regular channel conversions average 
    - "Off" if injected channel conversion data is lower than regular channel conversions
 - Error: In case of error, LED1 is toggling twice at a frequency of 1Hz.

@par Keywords

Analog, ADC, Analog to Digital, regular group, single conversion mode, HW trigger, 

@par Directory contents 

  - ADC/ADC_GroupsRegularInjected/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - ADC/ADC_GroupsRegularInjected/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - ADC/ADC_GroupsRegularInjected/Inc/main.h                  Header for main.c module  
  - ADC/ADC_GroupsRegularInjected/Src/stm32g4xx_it.c          Interrupt handlers
  - ADC/ADC_GroupsRegularInjected/Src/main.c                  Main program
  - ADC/ADC_GroupsRegularInjected/Src/system_stm32g4xx.c      STM32G4xx system source file


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
