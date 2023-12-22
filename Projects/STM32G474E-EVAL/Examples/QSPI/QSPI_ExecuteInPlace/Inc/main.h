/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    QSPI/QSPI_ExecuteInPlace/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32g474e_eval.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COUNTOF(__BUFFER__)        (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/* MT25QL512ABB Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE                       25  /* 512Mb */
#define QSPI_PAGE_SIZE                       256  /* 256 Bytes */

/***** Software RESET Operations *********************************************/
#define MT25QL512ABB_RESET_ENABLE_CMD                     0x66U   /* Reset Enable                                            */
#define MT25QL512ABB_RESET_MEMORY_CMD                     0x99U   /* Reset Memory                                            */

/***** READ ID Operations ****************************************************/
#define MT25QL512ABB_READ_ID_CMD                          0x9EU   /* Read IDentification                                     */
#define MT25QL512ABB_READ_ID_CMD2                         0x9FU   /* Read IDentification                                     */
#define MT25QL512ABB_MULTIPLE_IO_READ_ID_CMD              0xAFU   /* QPI ID Read                                             */
#define MT25QL512ABB_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5AU   /* Read Serial Flash Discoverable Parameter                */

/***** READ MEMORY Operations ************************************************/
#define MT25QL512ABB_READ_CMD                             0x03U   /* Normal Read 3/4 Byte Address                            */
#define MT25QL512ABB_FAST_READ_CMD                        0x0BU   /* Fast Read 3/4 Byte Address                              */
#define MT25QL512ABB_1I2O_FAST_READ_CMD                   0x3BU   /* Dual Output Fast Read 3/4 Byte Address                  */
#define MT25QL512ABB_2IO_FAST_READ_CMD                    0xBBU   /* Dual Input/Output Fast Read 3/4 Byte Address            */
#define MT25QL512ABB_1I4O_FAST_READ_CMD                   0x6BU   /* Quad Output Fast Read 3/4 Byte Address                  */
#define MT25QL512ABB_4IO_FAST_READ_CMD                    0xEBU   /* Quad Input/Output Fast Read 3/4 Byte Address            */
#define MT25QL512ABB_FAST_READ_DTR_CMD                    0x0DU   /* DTR Fast Read 3/4 Byte Address                          */
#define MT25QL512ABB_1I2O_FAST_READ_DTR_CMD               0x3DU   /* DTR Dual Output Fast Read 3/4 Byte Address              */
#define MT25QL512ABB_2IO_FAST_READ_DTR_CMD                0xBDU   /* DTR Dual Input/Output Fast Read 3/4 Byte Address        */
#define MT25QL512ABB_1I4O_FAST_READ_DTR_CMD               0x6DU   /* DTR Quad Output Fast Read 3/4 Byte Address              */
#define MT25QL512ABB_4IO_FAST_READ_DTR_CMD                0xEDU   /* DTR Quad Input/Output Fast Read 3/4 Byte Address        */
#define MT25QL512ABB_4IO_WORD_READ_CMD                    0xE7U   /* Quad Input/Output Word Read 3/4 Byte Address            */

/***** READ MEMORY Operations with 4-Byte Address ****************************/
#define MT25QL512ABB_4_BYTE_ADDR_READ_CMD                 0x13U   /* Normal Read 4 Byte address                              */
#define MT25QL512ABB_4_BYTE_ADDR_FAST_READ_CMD            0x0CU   /* Fast Read 4 Byte address                                */
#define MT25QL512ABB_4_BYTE_ADDR_1I2O_FAST_READ_CMD       0x3CU   /* Dual Output Fast Read 4 Byte address                    */
#define MT25QL512ABB_4_BYTE_ADDR_2IO_FAST_READ_CMD        0xBCU   /* Dual Input/Output Fast Read 4 Byte address              */
#define MT25QL512ABB_4_BYTE_ADDR_1I4O_FAST_READ_CMD       0x6CU   /* Quad Output Fast Read 4 Byte address                    */
#define MT25QL512ABB_4_BYTE_ADDR_4IO_FAST_READ_CMD        0xECU   /* Quad Input/Output Fast Read 4 Byte address              */
#define MT25QL512ABB_4_BYTE_ADDR_FAST_READ_DTR_CMD        0x0EU   /* DTR Fast Read 4 Byte Address                            */
#define MT25QL512ABB_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD    0xBEU   /* DTR Dual Input/Output Fast Read 4 Byte address          */
#define MT25QL512ABB_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD    0xEEU   /* DTR Quad Input/Output Fast Read 4 Byte address          */

/***** WRITE Operations ******************************************************/
#define MT25QL512ABB_WRITE_ENABLE_CMD                     0x06U   /* Write Enable                                            */
#define MT25QL512ABB_WRITE_DISABLE_CMD                    0x04U   /* Write Disable                                           */

/***** READ REGISTER Operations **********************************************/
#define MT25QL512ABB_READ_STATUS_REG_CMD                  0x05U   /* Read Status Register                                    */
#define MT25QL512ABB_READ_FLAG_STATUS_REG_CMD             0x70U   /* Read Flag Status Register                               */
#define MT25QL512ABB_READ_NONVOL_CFG_REG_CMD              0xB5U   /* Read NonVolatile Configuration Register                 */
#define MT25QL512ABB_READ_VOL_CFG_REG_CMD                 0x85U   /* Read Volatile Configuration Register                    */
#define MT25QL512ABB_READ_ENHANCED_VOL_CFG_REG_CMD        0x65U   /* Read Enhanced Volatile Configuration Register           */
#define MT25QL512ABB_READ_EXTENDED_ADDR_REG_CMD           0xC8U   /* Read Extended Address Register                          */
#define MT25QL512ABB_READ_GEN_PURP_REG_CMD                0x96U   /* Read Extended Address Register                          */

/***** WRITE REGISTER Operations *********************************************/
#define MT25QL512ABB_WRITE_STATUS_REG_CMD                 0x01U   /* Write Status Register                                   */
#define MT25QL512ABB_WRITE_NONVOL_CFG_REG_CMD             0xB1U   /* Write NonVolatile Configuration Register                */
#define MT25QL512ABB_WRITE_VOL_CFG_REG_CMD                0x81U   /* Write Volatile Configuration Register                   */
#define MT25QL512ABB_WRITE_ENHANCED_VOL_CFG_REG_CMD       0x61U   /* Write Enhanced Volatile Configuration Register          */
#define MT25QL512ABB_WRITE_EXTENDED_ADDR_REG_CMD          0xC5U   /* Write Extended Address Register                         */

/***** CLEAR FLAG STATUS REGISTER Operations *********************************/
#define MT25QL512ABB_CLEAR_FLAG_STATUS_REG_CMD            0x50U   /* Read Flag Status Register                               */

/***** PROGRAM Operations ****************************************************/
#define MT25QL512ABB_PAGE_PROG_CMD                        0x02U   /* Page Program 3/4 Byte Address                           */
#define MT25QL512ABB_DUAL_INPUT_FAST_PROG_CMD             0xA2U   /* Dual Input Fast Program 3/4 Byte Address                */
#define MT25QL512ABB_EXTENDED_DUAL_INPUT_FAST_PROG_CMD    0xD2U   /* Extended Dual Input Fast Program 3/4 Byte Address       */
#define MT25QL512ABB_QUAD_INPUT_FAST_PROG_CMD             0x32U   /* Quad Input Fast Program 3/4 Byte Address                */
#define MT25QL512ABB_EXTENDED_QUAD_INPUT_FAST_PROG_CMD    0x38U   /* Extended Quad Input Fast Program 3/4 Byte Address       */

/***** PROGRAM Operations with 4-Byte Address ********************************/
#define MT25QL512ABB_4_BYTE_ADDR_PAGE_PROG_CMD            0x12U   /* Page Program 4 Byte Address                             */
#define MT25QL512ABB_4_BYTE_ADDR_QUAD_INPUT_FAST_PROG_CMD 0x34U   /* Quad Input Fast Program 4 Byte Address                  */
#define MT25QL512ABB_4_BYTE_ADDR_QUAD_INPUT_EXTENDED_FAST_PROG_CMD 0x3EU /* Quad Input Extended Fast Program 4 Byte Address  */

/***** ERASE Operations ******************************************************/
#define MT25QL512ABB_SUBSECTOR_ERASE_32K_CMD              0x52U   /* SubSector Erase 32KB 3/4 Byte Address                   */
#define MT25QL512ABB_SUBSECTOR_ERASE_4K_CMD               0x20U   /* SubSector Erase 4KB 3/4 Byte Address                    */
#define MT25QL512ABB_SECTOR_ERASE_64K_CMD                 0xD8U   /* Sector Erase 64KB 3/4 Byte Address                      */
#define MT25QL512ABB_BULK_ERASE_CMD                       0x60U   /* Bulk Erase                                              */

/***** ERASE Operations with 4-Byte Address **********************************/
#define MT25QL512ABB_4_BYTE_ADDR_SECTOR_ERASE_64K_CMD     0xDCU   /* Sector Erase 64KB 4 Byte address                        */
#define MT25QL512ABB_4_BYTE_ADDR_SUBSECTOR_ERASE_4K_CMD   0x21U   /* SubSector Erase 4KB 4 Byte address                      */
#define MT25QL512ABB_4_BYTE_ADDR_SUBSECTOR_ERASE_32K_CMD  0x5CU   /* SubSector Erase 32KB 4 Byte address                     */

/***** SUSPEND/RESUME Operations *********************************************/
#define MT25QL512ABB_PROG_ERASE_SUSPEND_CMD               0x75U   /* Program/Erase suspend                                   */
#define MT25QL512ABB_PROG_ERASE_RESUME_CMD                0x7AU   /* Program/Erase resume                                    */

/***** ONE-TIME PROGRAMMABLE (OTP) Operations ********************************/
#define MT25QL512ABB_READ_OTP_ARRAY_CMD                   0x48U   /* Read OTP Array 3/4 Byte Address                         */
#define MT25QL512ABB_PROGRAM_OTP_ARRAY_CMD                0x42U   /* Program OTP Array 3/4 Byte Address                      */

/***** 4-BYTE ADDRESS MODE Operations ****************************************/
#define MT25QL512ABB_ENTER_4_BYTE_ADDR_MODE_CMD           0xB7U   /* Enter 4-Byte mode (3/4 Byte address commands)           */
#define MT25QL512ABB_EXIT_4_BYTE_ADDR_MODE_CMD            0xE9U   /* Exit 4-Byte mode (3/4 Byte address commands)            */

/***** QUAD PROTOCOL Operations **********************************************/
#define MT25QL512ABB_ENABLE_QSPI_CMD                      0x35U   /* Enable QPI; Enter QPI                                   */
#define MT25QL512ABB_RESET_QSPI_CMD                       0xF5U   /* Reset QPI; Exit QPI                                     */

/***** DEEP POWER-DOWN Operations ********************************************/
#define MT25QL512ABB_ENTER_DEEP_POWER_DOWN_CMD            0xB9U   /* Enter deep power down                                   */
#define MT25QL512ABB_RELEASE_FROM_DEEP_POWER_DOWN_CMD     0xABU   /* Release from Deep Power down                            */

/***** ADVANCED SECTOR PROTECTION Operations *********************************/
#define MT25QL512ABB_READ_SECTOR_PROTECTION_CMD           0x2DU   /* Read Sector Protection                                  */
#define MT25QL512ABB_PROGRAM_SECTOR_PROTECTION_CMD        0x2CU   /* Program Sector Protection                               */
#define MT25QL512ABB_READ_VOL_LOCK_BITS_CMD               0xE8U   /* Read Volatile Lock Bits 3/4 Byte Address                */
#define MT25QL512ABB_WRITE_VOL_LOCK_BITS_CMD              0xE5U   /* Write Volatile Lock Bits 3/4 Byte Address               */
#define MT25QL512ABB_READ_NONVOL_LOCK_BITS_CMD            0xE2U   /* Read NonVolatile Lock Bits 4 Byte Address               */
#define MT25QL512ABB_WRITE_NONVOL_LOCK_BITS_CMD           0xE3U   /* Write NonVolatile Lock Bits 4 Byte Address              */
#define MT25QL512ABB_ERASE_NONVOL_LOCK_BITS_CMD           0xE4U   /* Erase NonVolatile Lock Bits                             */
#define MT25QL512ABB_READ_GLOBAL_FREEZE_BIT_CMD           0xA7U   /* Read Global Freeze Bit                                  */
#define MT25QL512ABB_WRITE_GLOBAL_FREEZE_BIT_CMD          0xA6U   /* Write Global Freeze Bit                                 */
#define MT25QL512ABB_READ_PASSWORD_REGISTER_CMD           0x27U   /* Read Password                                           */
#define MT25QL512ABB_WRITE_PASSWORD_REGISTER_CMD          0x28U   /* Write Password                                          */
#define MT25QL512ABB_PASSWORD_UNLOCK_CMD                  0x29U   /* Unlock Password                                         */

/***** ADVANCED SECTOR PROTECTION Operations with 4-Byte Address *************/
#define MT25QL512ABB_4_BYTE_ADDR_READ_VOL_LOCK_BITS_CMD   0xE0U   /* Read Volatile Lock Bits 4 Byte Address                  */
#define MT25QL512ABB_4_BYTE_ADDR_WRITE_VOL_LOCK_BITS_CMD  0xE1U   /* Write Volatile Lock Bits 4 Byte Address                 */

/***** ADVANCED FUNCTION INTERFACE Operations ********************************/
#define MT25QL512ABB_INTERFACE_ACTIVATION_CMD             0x9BU   /* Interface Activation                                    */
#define MT25QL512ABB_CYCLIC_REDUNDANCY_CHECK_CMD          0x27U   /* Cyclic Redundancy Check                                 */


/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ              8
#define DUMMY_CLOCK_CYCLES_READ_QUAD         10

#define DUMMY_CLOCK_CYCLES_READ_DTR          6
#define DUMMY_CLOCK_CYCLES_READ_QUAD_DTR     8

/* End address of the QSPI memory */
#define QSPI_END_ADDR              (1 << QSPI_FLASH_SIZE)

/* Size of buffers */
#define BUFFERSIZE                 (COUNTOF(aTxBuffer) - 1)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
