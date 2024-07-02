/**
  @page SAI_AudioPlay  Description of the SAI audio play example

  @verbatim
  ******************************************************************************
  * @file    SAI/SAI_AudioPlay/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the SAI audio play example.
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

This example shows how to use the SAI HAL API to play an audio file using the DMA
circular mode and how to handle the buffer update.

      Plug a headphone to ear the sound  /!\ Take care of yours ears.
      Default volume is 30%.
      The audio file is played in loop
      @Note: Copy file 'audio.bin' (available in AudioFile) directly in the flash
      at @0x08040000 using ST-Link utility or STM32CubeProgrammer
Once started, LED1 is toggling on STM32G474E-EVAL1 Rev B.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Audio, SAI, DMA, Buffer Update, Play, Headphone, Audio protocol.

@par Directory contents

  - SAI/SAI_AudioPlay/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - SAI/SAI_AudioPlay/Src/main.c                  Main program
  - SAI/SAI_AudioPlay/Src/system_stm32g4xx.c      STM32G4xx system source file
  - SAI/SAI_AudioPlay/Src/stm32g4xx_it.c          Interrupt handlers
  - SAI/SAI_AudioPlay/Inc/main.h                  Main program header file
  - SAI/SAI_AudioPlay/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - SAI/SAI_AudioPlay/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - SAI/SAI_AudioPlay/AudioFile/audio.bin         Audio wave extract.

@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.

  - This example has been tested with STMicroelectronics STM32G474E-EVAL1 Rev B
    board and can be easily tailored to any other supported device
    and development board.

  - STM32G474E-EVAL1 Rev B Set-up :
    - Plug headset on Audio_Out JACK connector (CN26).

@par How to use it?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
