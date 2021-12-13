/**
  @page SPI_FullDuplex_ComDMA_Master SPI Full Duplex DMA example

  @verbatim
  ******************************************************************************
  * @file    SPI/SPI_FullDuplex_ComDMA_Master/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the SPI Full Duplex DMA example.
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

Data buffer transmission/reception between two boards via SPI using DMA.

Board: NUCLEO-G431KB RevA (embeds a STM32G431KBT6 device)
CLK Pin: PA5 (pin 8 on CN3 connector)
MISO Pin: PA6 (pin 7 on CN3 connector)
MOSI Pin: PA7 (pin 6 on CN3 connector)
   _________________________                       __________________________
  |           ______________|                      |______________           |
  |          |SPI1          |                      |          SPI1|          |
  |          |              |                      |              |          |
  |          |    CLK(PA.05)|______________________|CLK(PA.05)    |          |
  |          |              |                      |              |          |
  |          |   MISO(PA.06)|______________________|MISO(PA.06)   |          |
  |          |              |                      |              |          |
  |          |   MOSI(PA.07)|______________________|MOSI(PA.07)   |          |
  |          |              |                      |              |          |
  |          |______________|                      |______________|          |
  |      __                 |                      |                         |
  |     |__|                |                      |                         |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_STM32G4xx Master________|                      |_STM32G4xx Slave_________|

HAL architecture allows user to easily change code to move to Polling or IT 
mode. To see others communication modes please check following examples:
SPI\SPI_FullDuplex_ComPolling_Master and SPI\SPI_FullDuplex_ComPolling_Slave
SPI\SPI_FullDuplex_ComIT_Master and SPI\SPI_FullDuplex_ComIT_Slave

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 170 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK, 
GPIO, DMA and NVIC). You may update this function to change SPI configuration.

The SPI communication is then initiated.
The HAL_SPI_TransmitReceive_DMA() function allows the reception and the 
transmission of a predefined data buffer at the same time (Full Duplex Mode). 
If the Master board is used, the project SPI_FullDuplex_ComDMA_Master must be used.
If the Slave board is used, the project SPI_FullDuplex_ComDMA_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user connects the PA.12 (Arduino D2) to GND, SPI Master starts the
communication by sending aTxBuffer and receiving aRxBuffer through 
HAL_SPI_TransmitReceive_DMA(), at the same time SPI Slave transmits aTxBuffer 
and receives aRxBuffer through HAL_SPI_TransmitReceive_DMA(). 
The callback functions (HAL_SPI_TxRxCpltCallback and HAL_SPI_ErrorCallbackand) update 
the variable wTransferState used in the main function to check the transfer status.
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

STM32 board's LEDs can be used to monitor the transfer status:
 - LED2 toggles quickly on master board waiting PA.12 (Arduino D2) to be connected to GND
 - LED2 turns ON if transmission/reception is complete and OK.
 - LED2 toggles slowly when there is a timeout or an error in transmission/reception process.   

@note You need to perform a reset on Slave board, then perform it on Master board
      to have the correct behaviour of this example.
      
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Connectivity, SPI, Full-duplex, Interrupt, Transmission, Reception, Master, Slave, MISO, MOSI, DMA

@par Directory contents 

  - SPI/SPI_FullDuplex_ComDMA_Master/Inc/stm32g4xx_nucleo_conf.h     BSP configuration file
  - SPI/SPI_FullDuplex_ComDMA_Master/Inc/stm32g4xx_hal_conf.h   HAL configuration file
  - SPI/SPI_FullDuplex_ComDMA_Master/Inc/stm32g4xx_it.h         Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComDMA_Master/Inc/main.h                 Header for main.c module  
  - SPI/SPI_FullDuplex_ComDMA_Master/Src/stm32g4xx_it.c         Interrupt handlers
  - SPI/SPI_FullDuplex_ComDMA_Master/Src/main.c                 Main program
  - SPI/SPI_FullDuplex_ComDMA_Master/Src/system_stm32g4xx.c     stm32g4xx system source file
  - SPI/SPI_FullDuplex_ComDMA_Master/Src/stm32g4xx_hal_msp.c    HAL MSP file

@par Hardware and Software environment

  - This example runs on STM32G431KBTx devices.

  - This example has been tested with NUCLEO-G431KB board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-G431KB Set-up
    - Connect Master board PA5 (pin 8 on CN3 connector) to Slave Board PA5 (pin 8 on CN3 connector)
    - Connect Master board PA6 (pin 7 on CN3 connector) to Slave Board PA6 (pin 7 on CN3 connector)
    - Connect Master board PA7 (pin 6 on CN3 connector) to Slave Board PA7 (pin 6 on CN3 connector)
    - Connect Master board GND  to Slave Board GND

@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files (master project) and load your image into target memory
    o Load the project in Master Board
 - Rebuild all files (slave project) and load your image into target memory
    o Load the project in Slave Board
 - Run the example

 */
 
