/**
  @page HRTIM_PFC_TransitionMode HRTIM example
 
  @verbatim
  ******************************************************************************
  * @file    HRTIM_PFC_TransitionMode/readme.txt
  * @author  MCD Vertical Application Team
  * @brief   PFC - Transition mode HRTIM example
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  @endverbatim

@par Application Description

This example uses HRTIM to: 
- Generate a PWM as if it were controlling a PFC type converter in transition mode
- Generate the OverCurrent and ZeroCurrentDetection events of such system.  

The events and PWM can be probed on the board, and the events occurrence may be modified via
the board's joystick. The goal being to observe the influence of changing ZCD/OC events
on the PWM, and to showcase the usage of high resolution HRTIM for such a PFC topology. 

In real applications, only one HRTIM channel is required to control PFC in transition mode.
This corresponds to the usage that has been done of Timer D, which can be consulted directly
in CubeMX via the project .ioc file.

(c.f. to AN4539 for details about HRTIM usage for PFC control in transition mode)

In this example, three more channels are used to: 
- Channel E: generate ZCD event at a specific moment w.r.t. PWM set.
- Channel F: generate OC event at a specific moment w.r.t. PWM set.
- Channel C: output ZCD/OC blanking timeframe. 

@ note that the timer is used with a prescaling ratio of 16 (368ps precision, 2.72GHz counting clock). 

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond to have correct HAL operation.


@par Keywords

PFC, Transition Mode, Boundary Mode, STM32 Digital Power, HRTIMER usage

@par Directory contents
  - HRTIM/HRTIM_PFC_TransitionMode/Src/main.c                  Main program file
  - HRTIM/HRTIM_PFC_TransitionMode/Src/stm32g4xx_hal_msp.c     MSP Initialization file
  - HRTIM/HRTIM_PFC_TransitionMode/Src/stm32g4xx_it.c          Interrupt handlers file
  - HRTIM/HRTIM_PFC_TransitionMode/Src/system_stm32g4xx.c      STM32G4xx system clock configuration file
  - HRTIM/HRTIM_PFC_TransitionMode/Inc/main.h                  Main program header file
  - HRTIM/HRTIM_PFC_TransitionMode/Inc/stm32g4xx_hal_conf.h    HAL Library configuration file
  - HRTIM/HRTIM_PFC_TransitionMode/Inc/stm32g4xx_it.h          Interrupt handlers header file


@par Hardware and Software environment

- This example runs on STM32G474RETx devices.
- This example is tailored for B-G474E-DPOW1 board. 

IMPORTANT: Hardware Setup before running the program:
      1) Disconnect JP1.
      2) Probe the PFC PWM on PB14 with channel 1, trigger on rising edge with CC coupling.
	  
	  3) ZCD event is generated on PC9 and captured via PB5
		  --> Connect PC9 to PB5 (preferably with a female-female connector)
		  --> Probe either PC9 or PB5 to observe ZCD on scope.
		  
	  4) OC event is generated on PC7 and captured via PB9
		  --> Connect PC7 to PB9 (preferably with a female-female connector)
		  --> Probe either PC7 or PB9 to observe OC on scope.
		  
	  5) Blanking timeframe of OC/ZCD events can also be probed on PB13
	     (see use of Center button below)

Note: all measures must be taken with DC coupling

@par How to use it ?

In order to run the program, please do the following:
1. Open the "HRTIM_PFC_TransitionMode.ioc"
2. Select your preferred toolchain
3. Generate code.
4. Rebuild all files and flash target.
5. Press joystick to either:

--> Left: decrease OC event time w.r.t. PWM set.
--> Right: increase OC event time w.r.t. PWM set.
--> Down: decrease ZCD event time w.r.t. PWM set. 
--> Up: increase ZCD event time w.r.t. PWM set.

Note : ZCD rising edge cannot be configured at a time at which OC pulse is high 

--> Center: switch blanking window display on PB13

Note : PB13 default setting is to output the ZCD blanking window (applied with latch)
       Pressing the center allows to switch to OC blanking instead

	 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
