/**
  @page OPAMP_InternalFollower OPAMP example

  @verbatim
  ******************************************************************************
  * @file    Examples/OPAMP/OPAMP_InternalFollower/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the OPAMP_InternalFollower example.
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

This example provides a short description of how to configure the OPAMP in internal 
follower mode (unity gain). The signal applied on OPAMP non-inverting input is 
reproduced on OPAMP output.

Other peripherals are used in this example:
 - DAC generates continuously a signal on OPAMP1 non-inverting input (internal connection: no external connection needed)
 - ADC acquires the signal on OPAMP1 output (internal connection: no external connection needed)
   This signal can be observed on PA2 - pin 43 on connector CN6
   
   Note: ADC conversion results are stored into array "aADCxConvertedValues" (for debug: check into watch window)
         These results can be compared to reference waveform "Waveform_Ramp_12bits_5samples".

 - DAC and ADC are clock by the same timer TIM2: DAC on TIM2 rising edge, ADC on TIM2 falling edge
  (in order to have for each sample of DAC waveform a stabilization time of 1/2 TIM2 clock cycle, then ADC acquisition of this sample).


To run the example:
 - There is no external connection needed to run this example
 - Optionally, for this example purpose: pressing on User push-button enable/disable OPAMP
   to watch the impact on OPAMP output voltage and LED status.

STM32G474E-EVAL Rev B board's LED is be used to monitor the program execution status:
 - Normal operation: LED1 is turned-on/off in function of OPAMP output vs input voltage
   comparison status:
    - Turn-off if OPAMP output is different of non-inverting input
    - Turn-on if OPAMP output is equal to non-inverting input
 - Error: In case of error, LED1 is toggling at a frequency of 1Hz.

@par Keywords

OPAMP, Internal follower mode, DAC, ADC, Waveform.

@par Directory contents 

  - OPAMP/OPAMP_InternalFollower/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - OPAMP/OPAMP_InternalFollower/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - OPAMP/OPAMP_InternalFollower/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - OPAMP/OPAMP_InternalFollower/Inc/main.h                  Header for main.c module  
  - OPAMP/OPAMP_InternalFollower/Src/stm32g4xx_it.c          Interrupt handlers
  - OPAMP/OPAMP_InternalFollower/Src/main.c                  Main program
  - OPAMP/OPAMP_InternalFollower/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - OPAMP/OPAMP_InternalFollower/Src/system_stm32g4xx.c      STM32G4xx system source file

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

