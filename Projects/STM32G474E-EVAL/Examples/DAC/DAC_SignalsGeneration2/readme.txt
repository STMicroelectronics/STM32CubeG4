/**
  @page DAC_SignalsGeneration2 DAC Signals generation example
  
  @verbatim
  ******************************************************************************
  * @file    DAC/DAC_SignalsGeneration2/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DAC Signals generation example.
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
Use the DAC peripheral to generate several signals using the DMA
controller and the DAC internal wave generator.

Example configuration:

Example execution:
Form the start, the internal DAC wave generator is used to generate a Sawtooth wave.

For each press on User push-button, the signal to generate is changed between
    - Sawtooth waveform, amplitute: ~3V, frequency: ~1kHz.
    - Sine waveform using DMA transfer, amplitute: ~3V, frequency: ~1kHz.

For debug: variable to monitor with debugger watch window:
 - "ubSelectedWavesForm": DAC current wave to be generated

Connection needed:
Oscilloscope probe need to be connected to PA4 (pin 41 connector CN6) to observe waves generation effects.

Other peripherals used:
  1 GPIO for LED
  1 GPIO for analog output: PA4 (pin 41 connector CN6)
  1 GPIO for push button
  TIMER
  DMA

STM32G474E-EVAL Rev B board's LED can be used to monitor the process status:
  - LED3 is slowly blinking (1 sec. period) and example is stopped (using infinite loop)
  when there is an error during process.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Analog, DAC, Signals generation, DMA, Sawtooth, Sine, Waveform

@par Directory contents 

  - DAC/DAC_SignalsGeneration2/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - DAC/DAC_SignalsGeneration/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - DAC/DAC_SignalsGeneration/Inc/stm32g4xx_it.h          DAC interrupt handlers header file
  - DAC/DAC_SignalsGeneration/Inc/main.h                  Header for main.c module  
  - DAC/DAC_SignalsGeneration/Src/stm32g4xx_it.c          DAC interrupt handlers
  - DAC/DAC_SignalsGeneration/Src/main.c                  Main program
  - DAC/DAC_SignalsGeneration/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - DAC/DAC_SignalsGeneration/Src/system_stm32g4xx.c      STM32G4xx system source file

@par Hardware and Software environment  
  - This example runs on STM32G474QETx devices.
    
  - This example has been tested with STM32G474E-EVAL Rev B board and can be
    easily tailored to any other supported device and development board.

  - STM32G474E-EVAL Rev B Set-up :
      - Use User push-button connected to PC.13.
      - Use analog output PA4 (pin 41 connector CN6) 

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
  
 */

