/**
  @page TIM_Dithering TIM PWM Output with dithering example

  @verbatim
  ******************** (C) COPYRIGHT 2019 STMicroelectronics *******************
  * @file    TIM/TIM_Dithering/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_Dithering example
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

This example shows how to configure the TIM3 peripheral in PWM mode with dithering.

SystemCoreClock is set to 170 MHz for STM32G4xx Devices.

In this example TIM3 input clock (TIM3CLK) is set to APB1 clock (PCLK1),
    since APB1 prescaler is different from 1.
      TIM3CLK = PCLK1
      PCLK1 = HCLK
      => TIM3CLK = HCLK = SystemCoreClock
    To get TIM3 counter clock at 1500000 Hz (so that waveforms are easily observed with oscilloscope),
    the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock) /1500000 Hz) - 1

LD5 led is connected to TIM3_CH3, so that brightness represent the mean of output signal.

Example execution:
At initialization, period count (5 TIM3 ticks) and pulse count (1 TIM3 tick) are voluntarily very low,
so that dithering effect is easily visible on oscilloscope.
Dithering is activated but fractional part is null,
thus there is only regular pulses with 1/5 = 20% duty cycle.
And LD5 led brightness is quite low (about 20% of max intensity).
PWM waveforms can be displayed using an oscilloscope.
  TIM3_CH3
           _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _
         _| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |__

Then timer compare match is incremented regularly. This add 1 more cycle to x pulse out of 16 period (x fractional part = Compare modulo 16).
And LD5 led brightness is slightly increased.

Fractional part = 1 --> 1 pulse out of 16 periods is longer (1 more cycle). Dither duty cycle = (1+1)/5 = 40%:

  TIM3_CH3
           __    _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     __
         _|  |__| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___|  |_
          <---------------------------------------------------------------------------------------------->
           ^                                       16 periods
    Longer pulse
LD5 led brightness is slightly increased.

Fractional part = 2 --> 2 pulses out of 16 periods are longer (1 more cycle):
  TIM3_CH3
           __    _     _     _     _     _     _     _     __    _     _     _     _     _     _     _     __
         _|  |__| |___| |___| |___| |___| |___| |___| |___|  |__| |___| |___| |___| |___| |___| |___| |___|  |_
          <---------------------------------------------------------------------------------------------->
           ^                                               ^                  16 periods
       Longer pulse                                   Longer pulse
LD5 led brightness is slightly increased.

... and so on.

When fractional part reach 15, 15 pulses out of 16 have 40% duty cycle,
and only 1 out of 16 have 20% duty cycle.
Then compare match continue to be incremented, this means that integer part is incremented and fractional part is reset.
Thus all pulse are regular with a longer duration (40% duty cycle).
The cycle of fractional part increment restarts.
And LD5 led brightness continue to increased slightly.
When Pulse becomes longer than period, pulse is reset to restart from the beginning, (LD5 led brightness very low).

With dithering LD5 led brightness increases smoothly with (dither * period_count) 16 * 5 = 80 intermediate steps.

@note When dithering is activated, TIM3_CHTIM3_CH4 is configured in PWM output with a pulse 1/16 so that oscilloscope can be synchronized.
Pulse on CHTIM3_CH4 will appear every 16 periods.

  TIM3_CH4
           _                                                                                               _
         _| |_____________________________________________________________________________________________| |_
          <---------------------------------------------------------------------------------------------->
                                                  16 periods

If BUTTON_USER is pressed, dithering is deactivated. LD5 led brightness increases roughly with only 5 intermediate steps (= period_count).

 TIM3_CH3
           _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _
         _| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |__

 TIM3_CH3
           __    __    __    __    __    __    __    __    __    __    __    __    __    __    __    __    __
         _|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |__|  |_

... and so on.

If BUTTON_USER is pressed again dithering is reactivated.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Timer, TIM, Dithering, PWM, Frequency, Duty Cycle, Waveform, Oscilloscope, Output, Signal.

@par Directory contents

  - TIM/TIM_Dithering/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - TIM/TIM_Dithering/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_Dithering/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - TIM/TIM_Dithering/Inc/main.h                  Header for main.c module
  - TIM/TIM_Dithering/Src/stm32g4xx_it.c          Interrupt handlers
  - TIM/TIM_Dithering/Src/main.c                  Main program
  - TIM/TIM_Dithering/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - TIM/TIM_Dithering/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.
  - In this example, the clock is set to 170 MHz.

  - This example has been tested with STMicroelectronics STM32G474E-EVAL
    board and can be easily tailored to any other supported device
    and development board.

  - STM32G474E-EVAL Rev B Set-up
   Connect the following pins to an oscilloscope to monitor the waveforms:
        - TIM3_CHTIM3_CH3 : PC8 (pin 1 in CN5 connector)
        - TIM3_CHTIM3_CH4 : PC9 (pin 11 in CN5 connector) Oscilloscope synchronization in dithering mode (1 pulse every 16 periods)


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
