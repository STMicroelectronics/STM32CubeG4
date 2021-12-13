/**
  @page TIM_Dithering TIM PWM Output with dithering example

  @verbatim
  ******************** (C) COPYRIGHT 2018 STMicroelectronics *******************
  * @file    TIM/TIM_Dithering/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the TIM_Dithering example
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

This example shows how to configure the TIM2 peripheral in PWM mode with dithering.

SystemCoreClock is set to 170 MHz for STM32G4xx Devices.

In this example TIM2 input clock (TIM2CLK) is set to APB1 clock (PCLK1),
    since APB1 prescaler is different from 1.
      TIM2CLK = PCLK1
      PCLK1 = HCLK
      => TIM2CLK = HCLK = SystemCoreClock
    To get TIM2 counter clock at 1500000 Hz (so that waveforms are easily observed with oscilloscope),
    the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock) /1500000 Hz) - 1

LED2 is connected to TIM2_CH1, so that brightness represent the mean of output signal.

Example execution:
At initialization, period count (5 TIM2 ticks) and pulse count (1 TIM2 tick) are voluntarily very low,
so that dithering effect is easily visible on oscilloscope.
Dithering is activated but fractional part is null,
thus there is only regular pulses with 1/5 = 20% duty cycle.
And LED2 brightness is quite low (about 20% of max intensity).
PWM waveforms can be displayed using an oscilloscope.
  TIM2_CH1
           _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _
         _| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |__

Then timer compare match is incremented regularly. This add 1 more cycle to x pulse out of 16 period (x fractional part = Compare modulo 16).
And LED2 brightness is slightly increased.

Fractional part = 1 --> 1 pulse out of 16 periods is longer (1 more cycle). Dither duty cycle = (1+1)/5 = 40%:

  TIM2_CH1
           __    _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     __
         _|  |__| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___|  |_
          <---------------------------------------------------------------------------------------------->
           ^                                       16 periods
    Longer pulse
LED2 brightness is slightly increased.

Fractional part = 2 --> 2 pulses out of 16 periods are longer (1 more cycle):
  TIM2_CH1
           __    _     _     _     _     _     _     _     __    _     _     _     _     _     _     _     __
         _|  |__| |___| |___| |___| |___| |___| |___| |___|  |__| |___| |___| |___| |___| |___| |___| |___|  |_
          <---------------------------------------------------------------------------------------------->
           ^                                               ^                  16 periods
       Longer pulse                                   Longer pulse
LED2 brightness is slightly increased.

... and so on.

When fractional part reach 15, 15 pulses out of 16 have 40% duty cycle,
and only 1 out of 16 have 20% duty cycle.
Then compare match continue to be incremented, this means that integer part is incremented and fractional part is reset.
Thus all pulse are regular with a longer duration (40% duty cycle).
The cycle of fractional part increment restarts.
And LED2 brightness continue to increased slightly.
When Pulse becomes longer than period, pulse is reset to restart from the beginning, (LED2 brightness very low).

With dithering LED2 brightness increases smoothly with (dither * period_count) 16 * 5 = 80 intermediate steps.

@note When dithering is activated, TIM2_CHTIM2_CH2 is configured in PWM output with a pulse 1/16 so that oscilloscope can be synchronized.
Pulse on CHTIM2_CH2 will appear every 16 periods.

  TIM2_CH2
           _                                                                                               _
         _| |_____________________________________________________________________________________________| |_
          <---------------------------------------------------------------------------------------------->
                                                  16 periods

If BUTTON_USER is pressed, dithering is deactivated. LED2 brightness increases roughly with only 5 intermediate steps (= period_count).

 TIM2_CH1
           _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _     _
         _| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |___| |__

 TIM2_CH1
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

Timer, Dithering, PWM, Frequency, Duty Cycle, Waveform, Oscilloscope, Output, Signal.

@par Directory contents

  - TIM/TIM_Dithering/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - TIM/TIM_Dithering/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - TIM/TIM_Dithering/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - TIM/TIM_Dithering/Inc/main.h                  Header for main.c module
  - TIM/TIM_Dithering/Src/stm32g4xx_it.c          Interrupt handlers
  - TIM/TIM_Dithering/Src/main.c                  Main program
  - TIM/TIM_Dithering/Src/stm32g4xx_hal_msp.c     HAL MSP file
  - TIM/TIM_Dithering/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G491RETx devices.
  - In this example, the clock is set to 170 MHz.

  - This example has been tested with STMicroelectronics NUCLEO-G491RE
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-G491RE RevA Set-up
   Connect the following pins to an oscilloscope to monitor the waveforms:
        - TIM2_CHTIM2_CH1 : PA5 (pin 11 in CN10 connector)
        - TIM2_CHTIM2_CH2 : PA1 (pin 30 in CN7 connector) Oscilloscope synchronization in dithering mode (1 pulse every 16 periods)


@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
