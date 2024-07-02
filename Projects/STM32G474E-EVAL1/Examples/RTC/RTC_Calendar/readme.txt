/**
  @page RTC_Calendar RTC Calendar Example
  
  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_Calendar/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC Calendar example.
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

Configuration of the calendar using the RTC HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

The RTC peripheral configuration is ensured by the HAL_RTC_Init() function.
This later is calling the HAL_RTC_MspInit()function which core is implementing
the configuration of the needed RTC resources according to the used hardware (CLOCK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

LSI oscillator clock is used as RTC clock source. 

HAL_RTC_SetTime()and HAL_RTC_SetDate() functions are then called to initialize the 
time and the date.

A key value is written in backup data register 1 to indicate if the RTC is already configured.  
The RTC is in the backup (BKP) domain, still powered by VBAT when VDD is switched off,
so the RTC configuration is not lost if a battery is connected to the VBAT pin. 
The program behaves as follows:

1. After startup the program checks the backup data register 0 value:
    - BKP_DR0 value not correct: (RTC_BKP_DR0 value is not correct or has not yet
      been programmed when the program is executed for the first time) the RTC is
      configured and internal time stamp is enabled.
    
    - BKP_DR0 value correct: this means that the RTC is configured and the time
      and date are displayed on Debugger.

2. When a reset (except power on reset) occurs the BKP domain is not reset and the RTC 
   configuration is not lost.
   
3. When power on reset occurs:
    - If a battery is connected to the VBAT pin: the BKP domain is not reset and
      the RTC configuration is not lost. LED3 is ON.

    - If no battery is connected to the VBAT pin: the BKP domain is reset and the
      RTC configuration is lost.

LED1 is turned ON when the RTC configuration is done correctly.


The current time and date are updated and displayed on the debugger in aShowTime 
and aShowDate variables.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, RTC, Calendar, Backup Domain, Reset

@par Directory contents 

  - RTC/RTC_Calendar/Inc/stm32g474e_eval_conf.h     BSP configuration file
  - RTC/RTC_Calendar/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - RTC/RTC_Calendar/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - RTC/RTC_Calendar/Inc/main.h                  Header for main.c module  
  - RTC/RTC_Calendar/Src/stm32g4xx_it.c          Interrupt handlers
  - RTC/RTC_Calendar/Src/main.c                  Main program
  - RTC/RTC_Calendar/Src/stm32g4xx_hal_msp.c     HAL MSP module
  - RTC/RTC_Calendar/Src/system_stm32g4xx.c      STM32G4xx system source file


@par Hardware and Software environment

  - This example runs on STM32G474QETx Devices.
  - This example has been tested with STMicroelectronics STM32G474E-EVAL1 Rev B
    board and can be easily tailored to any other supported device and 
    development board. 
  - STM32G474E-EVAL1 Rev B Set-up
  - Make sure that  is in position  to connect 3V battery to VBAT pin.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 */
