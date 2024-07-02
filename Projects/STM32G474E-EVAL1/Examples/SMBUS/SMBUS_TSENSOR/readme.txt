/**
  @page SMBUS_TSENSOR SMBUS Temperature Sensor example
  
  @verbatim
  ******************************************************************************
  * @file    SMBUS/SMBUS_TSENSOR/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the SMBUS Temperature Sensor example.
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

This example shows how to ensure SMBUS Data buffer transmission and reception with 
IT. The communication is done with a SMBUS temperature sensor.

Board: STM32G474E-EVAL1
SCL Pin: PG7
SDA Pin: PG8
SMB alert Pin: PG6
   __________________________________________________________________________                        
  |           ______________                        ______________           |
  |          | I2C3         |                      |   TSENSOR    |          |
  |          |              |                      |              |          |
  |          |          SCL |______________________|CLOCK         |          |
  |          |              |                      |              |          |
  |          |              |                      |              |          |
  |          |              |                      |              |          |
  |          |          SDA |______________________|DATA          |          |
  |          |              |                      |              |          |
  |          |    SMB alert |______________________|EVENT         |          |
  |          |______________|                      |______________|          |
  |      __                                                                  |
  |     |__|                                                                 |
  |     User push-button                                                      |
  |                                                                          |
  |__________________________________________________________________________|
      STM32G474E-EVAL1

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 150 MHz.

The temperature sensor configuration is ensured by TSENSOR_Init() which verify that
temperature sensor is ready to communicate by calling TSENSOR_IsReady(), then
set the configuration using HAL_SMBUS_Master_Transmit_IT() and HAL_SMBUS_EnableAlert_IT()

The SMBUS peripheral configuration is ensured by the HAL_SMBUS_Init()
and HAL_SMBUS_IsDeviceReady() called respectively in TSENSOR_Init() function.
This later is calling the HAL_SMBUS_MspInit()function which core is implementing
the configuration of the needed SMBUS resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change SMBUS configuration.

The SMBUS/Temperature Sensor communication is then initiated.

Then at each loop in main program a read and a display on LCD of the current temperature
is done each second.
An specific warning message is display on LCD in case of an alert from temperature sensor
is detected.

This management is perform using the TSENSOR_ReadTemp(), TSENSOR_AlerteResponseAddressRead()
and TSENSOR_ReadStatus() functions allow respectively the read of the current temperature,
 get the address of temperature sensor 
in alert and get the temperature sensor status.

STM32G474E-EVAL1's Messages and LEDs can be used to monitor the temperature sensor status:
 - LED3 is ON when there is an error during communication with the temperature sensor.
 - In case of the Temperature high limit exceeded, this blink warning message is displayed on the LCD
 until alert limit disappear :
                                    Temperature Limit High
                                       has been exceeded

 - In case of the Temperature is at or blow low limit,a blink warning message is displayed on the LCD
 until alert limit disappear :
                                      Temperature is at or
                                       below the Low Limit

@note TSENSOR_SMBUS instance used and associated resources can be updated in "stm32g4xx_tsensor.c"
and "main.h" files depending hardware configuration used.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The example need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

SMBUS, Temperature, Sensor, I2C, Communication, Transmission, Reception, Interrupt, LCD.

@par Directory contents 

  - SMBUS/SMBUS_TSENSOR/Inc/stm32g4xx_hal_conf.h    HAL configuration file
  - SMBUS/SMBUS_TSENSOR/Inc/stm32g4xx_it.h          Interrupt handlers header file
  - SMBUS/SMBUS_TSENSOR/Inc/main.h                  Header for main.c module  
  - SMBUS/SMBUS_TSENSOR/Inc/stm32g4xx_tsensor.h     Temperature Sensor driver header file
  - SMBUS/SMBUS_TSENSOR/Src/stm32g4xx_it.c          Interrupt handlers
  - SMBUS/SMBUS_TSENSOR/Src/main.c                  Main program
  - SMBUS/SMBUS_TSENSOR/Src/system_stm32g4xx.c      STM32G4xx system source file
  - SMBUS/SMBUS_TSENSOR/Src/stm32g4xx_hal_msp.c     HAL MSP file    
  - SMBUS/SMBUS_TSENSOR/Src/stm32g4xx_tsensor.c     Temperature Sensor driver file

@par Hardware and Software environment

  - This example runs on STM32G474QETx devices.
    
  - This example has been tested with STM32G474E-EVAL1 board and can be
    easily tailored to any other supported device and development board.    

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Download and Debug
 - Run program: Debug->Go(F5) 


 */
