/**
  @page COMP_CompareGpioVsVrefInt_OutputGpio COMP example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/COMP/COMP_CompareGpioVsVrefInt_OutputGpio/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the COMP_CompareGpioVsVrefInt_OutputGpio Example.
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

This example shows how to configure the COMP peripheral to compare the external
voltage applied on a specific pin with an internal reference. 

In this example, the comparator input is connected on the pin PB1 (pin 33 on connector CN6) the user shall vary the voltage on it
by actioning potentiometer RV2.
VREFINT internal voltage (about 1.22V) is connected to comparator other input.
The comparator output is connected to the pin PF4 (pin 57 on connector CN6).

LED1 state:
  - "ON" when comparator output is high.
  - "OFF" when comparator output is low.
  - toggling when an error occurred.

@note Care must be taken when using HAL_Delay(), this function provides 
      accurate delay (in milliseconds) based on variable incremented in SysTick ISR. 
      This implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower) than the 
      peripheral interrupt. Otherwise the caller ISR process will be blocked. 
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set 
      to 1 millisecond to have correct HAL operation.

@par Keywords

comparator, voltage compare.

@par Directory contents 

  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Inc/stm32g4xx_it.h          COMP interrupt handlers header file
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Inc/main.h                  Header for main.c module
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Src/stm32g4xx_it.c          COMP interrupt handlers
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Src/main.c                  Main program
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Src/stm32g4xx_hal_msp.c     HAL MSP file 
  - COMP/COMP_CompareGpioVsVrefInt_OutputGpio/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment 

  - This example runs on STM32G474xx devices.

  - This example has been tested with STM32G474E-EVAL Rev B board and can be
    easily tailored to any other supported device and development board.

  - Connect jumper JP5 on 2-3 position (LDR)
  - Connect a wire between JP5 pin 1 and PB1


    
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 */