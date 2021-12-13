/**
  @page TIM_EncoderIndex_PulseOnCompare TIM Encoder Index and Pulse On Compare example

  @verbatim
  ******************** (C) COPYRIGHT 2018 STMicroelectronics *******************
  * @file    TIM/TIM_EncoderIndex_PulseOnCompare/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_EncoderIndex_PulseOnCompare example
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This example shows how to configure the TIM3 peripheral in encoder mode with index
and generate a pulse on a certain value of encoder interface counter with pulse on compare.

     -------------                               -------------
    |             |CH1:PC0          CH1:PA6     |             |
    |             |---------------------------->|             |
    |       TIM1  |                             |    TIM3     |
    |             |CH2:PC1          CH2:PA4     |             |
    | Quadrature  |---------------------------->| Quadrature  |
    | encoder     |                             | encoder     |
    | emulator    |                             | interface   |CH3:PB0
    |             |                             |             |----------->   Pulse On Compare
    |             |GPIO:PC2         ETR:PD2     |             |
    |       Index |---------------------------->|             |
    |_____________|                             |             |
                                                 -------------
               Encoder with index and pulse on compare example description


To emulate a quadrature encoder, TIM1 is configured in toggle mode to generate
2 quadrature signals on PC0 and PC1 at 250Hz.
So encoder clock = 250 * both edge * both signal = 1kHz.
GPIO PC2 is used to emulate index. Index occurs at each update event determined by repetition counter.

TIM3 is configured in encoder mode interface, counting on TI1 and TI2.
TIM3 channel 3 is used as pulse on compare. It is configured so that compare match occurs only when index is inactive.

Example execution:
At initialization, quadrature encoder emulator is active but index emulation is deactivated.
Thus encoder interface counts up and is able to reach value to trig pulse on compare.
Pulse on compare waveform can be displayed using an oscilloscope on PB0.
Pulse duration is PW * 2^(PWPRSC)/SystemCoreClock = (255 * 128) / 170000000.

If BUTTON_USER is pressed, Index is activated. Thus encoder interface counter is reset before reaching pulse on compare match.
Pulse waveforms is no more present.

If BUTTON_USER is pressed again Index is deactivated.

The Encoder CH1/CH2, Index(ETR) and Pulse On Compare waveforms can be displayed using an oscilloscope.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Timers, TIM, Output, signals, encoder mode, Pulse On Compare, Oscilloscope, Waveform, Encoder index.

@par Directory contents

  - TIM/TIM_EncoderIndex_PulseOnCompare/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_EncoderIndex_PulseOnCompare/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_EncoderIndex_PulseOnCompare/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - TIM/TIM_EncoderIndex_PulseOnCompare/Inc/main.h                  Header for main.c module
  - TIM/TIM_EncoderIndex_PulseOnCompare/Src/stm32g4xx_it.c          Interrupt handlers
  - TIM/TIM_EncoderIndex_PulseOnCompare/Src/main.c                  Main program
  - TIM/TIM_EncoderIndex_PulseOnCompare/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - TIM/TIM_EncoderIndex_PulseOnCompare/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G491RETx devices.

  - This example has been tested with NUCLEO-G491RE board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G491RE Set-up
    - Connect PC.00 (A5 - pin 38 in CN7 connector) TIM1 Channel 1
      to PA.06 (D12 - pin 13 in CN10 connector) TIM3 Channel 1.
    - Connect PC.01 (A4 - pin 36 in CN7 connector) TIM1 Channel 2
      to PA.04 (A2 - pin 32 in CN7 connector) TIM3 Channel 2.
    - Connect PC.02(pin 35 in CN7 connector) GPIO used for Index
      to PD.02 (pin 4 in CN7 connector) TIM3 ETR.
    - Connect PB.00 (A3 - pin 34 in CN7 connector) TIM3 Channel 3
      to oscilloscope to display pulse on compare waveform.


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5)

 */
