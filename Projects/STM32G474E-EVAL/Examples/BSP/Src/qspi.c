/**
  ******************************************************************************
  * @file    BSP/Src/qspi.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the QSPI in the 
  *          STM32G474E EVAL driver
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>

/** @addtogroup STM32G4xx_HAL_Examples
* @{
*/

/** @addtogroup BSP
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE       256U // = MT25QL512ABB_PAGE_SIZE
#define BLOCK_SIZE        MT25QL512ABB_SECTOR_64K
#define NB_BLOCK          1U

#define QPI_MMP_ADDRESS   ((uint32_t *) 0x90000000)
#define TEST_VALUE_START  0x43 // Test pattern start value. Following data ++...

#define INTERFACE_MODE_NUMBER  7
#define TRANSFER_RATE_NUMBER   2
#define DUALFLASH_MODE_NUMBER  2
#define FLASH_ID_NUMBER        2

/* Private macro -------------------------------------------------------------*/
#define Xpos_START 20
#define Ypos_START 80
#define FONT       Font12
#define CHAR_LINE  Font12.Height
#define CHAR_WIDE  Font12.Width
#define NEXT_LINE  {Xpos = Xpos_START; Ypos += CHAR_LINE;}

/* Private variables ---------------------------------------------------------*/
uint8_t qspi_aTxBuffer[BUFFER_SIZE];
uint8_t qspi_aRxBuffer[BUFFER_SIZE];
uint8_t qspi_aErasedBuffer[BUFFER_SIZE];
uint32_t qspi_MMPBuffer[BUFFER_SIZE];

static BSP_QSPI_Info_t pQSPI_Info;       // Flash information

static uint16_t Xpos = Xpos_START, Ypos = Ypos_START;

BSP_QSPI_Init_t Flash;

BSP_QSPI_Interface_t InterfaceMode[INTERFACE_MODE_NUMBER] = {
  BSP_QSPI_SPI_MODE,
  BSP_QSPI_SPI_1I2O_MODE,
  BSP_QSPI_SPI_2IO_MODE,
  BSP_QSPI_SPI_1I4O_MODE,
  BSP_QSPI_SPI_4IO_MODE,
  BSP_QSPI_DPI_MODE,
  BSP_QSPI_QPI_MODE
};

char* InterfaceModeText[INTERFACE_MODE_NUMBER] =
{
  "SPI_MODE",
  "SPI_1I2O_MODE",
  "SPI_2IO_MODE",
  "SPI_1I4O_MODE",
  "SPI_4IO_MODE",
  "DPI_MODE",
  "QPI_MODE"
};

BSP_QSPI_Transfer_t TransferRate[TRANSFER_RATE_NUMBER] = {
  BSP_QSPI_STR_TRANSFER,
  BSP_QSPI_DTR_TRANSFER
};

char* TransferRateText[TRANSFER_RATE_NUMBER] =
{
  "STR",
  "DTR"
};

BSP_QSPI_DualFlash_t DualFlashMode[DUALFLASH_MODE_NUMBER] = {
  BSP_QSPI_DUALFLASH_DISABLE,
  BSP_QSPI_DUALFLASH_ENABLE
};

char* DualFlashModeText[DUALFLASH_MODE_NUMBER] =
{
  "SingleFlash",
  "DualFlash"
};

uint32_t FlashId[FLASH_ID_NUMBER] = {
  BSP_QSPI_FLASH_ID_1,
  BSP_QSPI_FLASH_ID_2
};

/* Private function prototypes -----------------------------------------------*/
static void QSPI_SetHint(void);
static void QSPI_Indirect_Mode(void);
static void QSPI_MemoryMapped_Mode(void);
static void QSPI_FlashId(void);
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t RMABuffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength, uint32_t *ErrorAd);


/* Private functions ---------------------------------------------------------*/
/**
* @brief  QSPI tests
* @param  None
* @retval None
*/
int32_t QSPI_demo(void)
{
  QSPI_SetHint();
  UTIL_LCD_SetFont(&FONT);
  Xpos = Xpos_START;
  Ypos = Ypos_START;

  /* Read & check the QSPI info */
  /* Initialize the structure */
  pQSPI_Info.FlashSize          = (uint32_t)0x00;
  pQSPI_Info.EraseSectorSize    = (uint32_t)0x00;
  pQSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
  pQSPI_Info.ProgPageSize       = (uint32_t)0x00;
  pQSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
  BSP_QSPI_GetInfo(0, &pQSPI_Info);

  QSPI_Indirect_Mode();
  QSPI_MemoryMapped_Mode();
  QSPI_FlashId();

  while(1)
  {
    if(CheckForUserInput() > 0)
    {
      ButtonState = 0;
      return 0;
    }
  }
}

static void QSPI_Indirect_Mode(void)
{
  uint32_t Offset;
  uint32_t i, j, k, l;
  uint8_t flash_id[6];
  uint32_t error_code = 0;

  /* Fill the buffer for write operation **************************************/
  Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, TEST_VALUE_START);
  memset(qspi_aRxBuffer, 0, BUFFER_SIZE);

  /* Loop over dualflash modes ************************************************/
  for(l = 0; l < DUALFLASH_MODE_NUMBER; l++)
  {
    /* Loop over transfer rates ***********************************************/
    for(k = 0; k < TRANSFER_RATE_NUMBER; k++)
    {
      /* Loop over interface modes ********************************************/
      for(j = 0; j < INTERFACE_MODE_NUMBER; j++)
      {
        /* QSPI device configuration */
        Flash.InterfaceMode = InterfaceMode[j];
        Flash.TransferRate  = TransferRate[k];
        Flash.DualFlashMode = DualFlashMode[l];

        BSP_QSPI_DeInit(0);

        if(BSP_QSPI_Init(0, &Flash) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Init: FAILED.", LEFT_MODE);
        }
        else if(BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Get Status: FAILED.", LEFT_MODE);
        }
        else if(BSP_QSPI_ReadID(0, flash_id) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read ID: FAILED.", LEFT_MODE);
        }
        else
        {
          if(Flash.DualFlashMode == MT25QL512ABB_DUALFLASH_DISABLE)
          {
            if((flash_id[0] != 0x20) || (flash_id[1] != 0xBA) || (flash_id[2] != 0x20))
            {
              UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read ID: FAILED.", LEFT_MODE);
            }
          }
          else
          {
            if((flash_id[0] != 0x20) || (flash_id[1] != 0x20) || (flash_id[2] != 0xBA) ||
               (flash_id[3] != 0xBA) || (flash_id[4] != 0x20) || (flash_id[5] != 0x20))
            {
              UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read ID: FAILED.", LEFT_MODE);
            }
          }
        }

        for(i = 0; i < NB_BLOCK; i++)
        {
          if(BSP_QSPI_EraseBlock(0, i*BLOCK_SIZE, MT25QL512ABB_ERASE_64K) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Erase: FAILED.", LEFT_MODE);
          }
          while(BSP_QSPI_GetStatus(0) == BSP_ERROR_BUSY);
        }

        if(BSP_QSPI_ConfigFlash(0, InterfaceMode[(j+1)%INTERFACE_MODE_NUMBER], TransferRate[(k+1)%TRANSFER_RATE_NUMBER]) != BSP_ERROR_NONE) Error_Handler();

        for(i = 0; i < (NB_BLOCK*BLOCK_SIZE/BUFFER_SIZE); i++)
        {
          if(BSP_QSPI_Write(0, qspi_aTxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Write: FAILED.", LEFT_MODE);
          }

          if(BSP_QSPI_Read(0, qspi_aRxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read: FAILED.", LEFT_MODE);
          }

          if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
          {
            error_code++;
          }
        }
      }

      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Indirect", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(Xpos+(10*CHAR_WIDE), Ypos, (uint8_t*)TransferRateText[k], LEFT_MODE);
      UTIL_LCD_DisplayStringAt(Xpos+(14*CHAR_WIDE), Ypos, (uint8_t*)DualFlashModeText[l], LEFT_MODE);
      UTIL_LCD_DisplayStringAt(Xpos+(26*CHAR_WIDE), Ypos, (uint8_t*)":", LEFT_MODE);
      if(error_code == 0)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
        UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"OK", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"FAILED", LEFT_MODE);
        error_code = 0;
      }
      NEXT_LINE
    }
  }

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
}

static void QSPI_MemoryMapped_Mode(void)
{
  uint32_t Offset;
  uint32_t i, j, k, l;
  uint32_t *QSPI_MMP_pointer;
  uint32_t error_code = 0;

   /* Fill the buffer for write operation *************************************/
  Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, TEST_VALUE_START);
  memset(qspi_aRxBuffer, 0, BUFFER_SIZE);

  /* Loop over dualflash modes ************************************************/
  for(l = 0; l < DUALFLASH_MODE_NUMBER; l++)
  {
    /* Loop over transfer rates ***********************************************/
    for(k = 0; k < TRANSFER_RATE_NUMBER; k++)
    {
      /* Loop over interface modes ********************************************/
      for(j = 0; j < INTERFACE_MODE_NUMBER; j++)
      {
        /* QSPI device configuration */
        Flash.InterfaceMode = InterfaceMode[j];
        Flash.TransferRate  = TransferRate[k];
        Flash.DualFlashMode = DualFlashMode[l];

        BSP_QSPI_DeInit(0);

        if(BSP_QSPI_Init(0, &Flash) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Init: FAILED.", LEFT_MODE);
        }
        else if(BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Get Status: FAILED.", LEFT_MODE);
        }

        for(i = 0; i < NB_BLOCK; i++)
        {
          if(BSP_QSPI_EraseBlock(0, i*BLOCK_SIZE, MT25QL512ABB_ERASE_64K) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Erase:  FAILED.", LEFT_MODE);
          }
          while(BSP_QSPI_GetStatus(0) == BSP_ERROR_BUSY);
        }

        for(i = 0; i < (NB_BLOCK*BLOCK_SIZE/BUFFER_SIZE); i++)
        {
          if(BSP_QSPI_Write(0, qspi_aTxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
          {
            UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Write: FAILED.", LEFT_MODE);
          }
        }

        if(BSP_QSPI_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"MemoryMap enable: FAILED.", LEFT_MODE);
        }

        QSPI_MMP_pointer = (uint32_t*)(QPI_MMP_ADDRESS);
        for(i = 0; i < (NB_BLOCK*BLOCK_SIZE/BUFFER_SIZE); i++)
        {
          for(int k = 0; k < BUFFER_SIZE/4; k++)
          {
            qspi_MMPBuffer[k] = *(QSPI_MMP_pointer++);
            qspi_aRxBuffer[4*k] = qspi_MMPBuffer[k] & 0xFF;
            qspi_aRxBuffer[4*k + 1] = (qspi_MMPBuffer[k] >> 8) & 0xFF;
            qspi_aRxBuffer[4*k + 2] = (qspi_MMPBuffer[k] >>16) & 0xFF;
            qspi_aRxBuffer[4*k + 3] = (qspi_MMPBuffer[k] >>24) & 0xFF;
          }

          if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
          {
            error_code++;
          }
        }
        if(BSP_QSPI_DisableMemoryMappedMode(0) != BSP_ERROR_NONE)
        {
          UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"MemoryMap disable: FAILED.", LEFT_MODE);
        }
      }

      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"MemoryMap", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(Xpos+(10*CHAR_WIDE), Ypos, (uint8_t*)TransferRateText[k], LEFT_MODE);
      UTIL_LCD_DisplayStringAt(Xpos+(14*CHAR_WIDE), Ypos, (uint8_t*)DualFlashModeText[l], LEFT_MODE);
      UTIL_LCD_DisplayStringAt(Xpos+(26*CHAR_WIDE), Ypos, (uint8_t*)":", LEFT_MODE);
      if(error_code == 0)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
        UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"OK", LEFT_MODE);
      }
      else
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"FAILED", LEFT_MODE);
        error_code = 0;
      }
      NEXT_LINE
    }
  }

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
}

static void QSPI_FlashId(void)
{
  uint32_t Offset;
  uint32_t i, j;
  uint32_t error_code = 0;

  /* Fill the buffer for write operation **************************************/
  Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, TEST_VALUE_START);
  memset(qspi_aErasedBuffer, 0xFF, BUFFER_SIZE);

  /* QSPI device configuration */
  Flash.InterfaceMode = BSP_QSPI_SPI_MODE;
  Flash.TransferRate  = BSP_QSPI_STR_TRANSFER;
  Flash.DualFlashMode = BSP_QSPI_DUALFLASH_DISABLE;

  BSP_QSPI_DeInit(0);

  /* By default FLASH_ID_1 is selected */
  if(BSP_QSPI_Init(0, &Flash) != BSP_ERROR_NONE)
  {
    UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Init: FAILED.", LEFT_MODE);
  }

  /* Loop over Flash Id *******************************************************/
  for(j = 0; j < FLASH_ID_NUMBER; j++)
  {
    if(BSP_QSPI_SelectFlashID(0, FlashId[j]) != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"SelectFlashID: FAILED.", LEFT_MODE);
    }
    else if(BSP_QSPI_GetStatus(0) != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Get Status: FAILED.", LEFT_MODE);
    }

    for(i = 0; i < NB_BLOCK; i++)
    {
      /* Erase blocks *********************************************************/
      if(BSP_QSPI_EraseBlock(0, i*BLOCK_SIZE, MT25QL512ABB_ERASE_64K) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Erase: FAILED.", LEFT_MODE);
      }
      while(BSP_QSPI_GetStatus(0) == BSP_ERROR_BUSY);
    }

    for(i = 0; i < (NB_BLOCK*BLOCK_SIZE/BUFFER_SIZE); i++)
    {
      /* Read blocks **********************************************************/
      if(BSP_QSPI_Read(0, qspi_aRxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
      {
        UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read: FAILED.", LEFT_MODE);
      }

      /* Check blocks empty ***************************************************/
      if(RMABuffercmp(qspi_aRxBuffer, qspi_aErasedBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
      {
        error_code++;
      }
    }
  }

  /* Select Flash Id 1 ********************************************************/
  if(BSP_QSPI_SelectFlashID(0, BSP_QSPI_FLASH_ID_1) != BSP_ERROR_NONE) Error_Handler();

  for(i = 0; i < (NB_BLOCK*BLOCK_SIZE/BUFFER_SIZE); i++)
  {
    /* Write blocks ***********************************************************/
    if(BSP_QSPI_Write(0, qspi_aTxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Write: FAILED.", LEFT_MODE);
    }

    /* Read blocks ************************************************************/
    if(BSP_QSPI_Read(0, qspi_aRxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read: FAILED.", LEFT_MODE);
    }

    /* Check blocks written ***************************************************/
    if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
    {
      error_code++;
    }
  }

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Flash ID 1", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(Xpos+(26*CHAR_WIDE), Ypos, (uint8_t*)":", LEFT_MODE);
  if(error_code == 0)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"OK", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"FAILED", LEFT_MODE);
    error_code = 0;
  }
  NEXT_LINE

  /* Select Flash Id 2 ********************************************************/
  if(BSP_QSPI_SelectFlashID(0, BSP_QSPI_FLASH_ID_2) != BSP_ERROR_NONE) Error_Handler();

  for(i = 0; i < (NB_BLOCK*BLOCK_SIZE/BUFFER_SIZE); i++)
  {
    /* Read blocks ************************************************************/
    if(BSP_QSPI_Read(0, qspi_aRxBuffer, i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Read: FAILED.", LEFT_MODE);
    }

    /* Check blocks empty *****************************************************/
    if(RMABuffercmp(qspi_aRxBuffer, qspi_aErasedBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
    {
      error_code++;
    }
  }

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(Xpos, Ypos, (uint8_t*)"Flash ID 2", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(Xpos+(26*CHAR_WIDE), Ypos, (uint8_t*)":", LEFT_MODE);
  if(error_code == 0)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"OK", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(Xpos+(28*CHAR_WIDE), Ypos, (uint8_t*)"FAILED", LEFT_MODE);
    error_code = 0;
  }
  NEXT_LINE
}

/**
* @brief  Display QSPI Demo Hint
* @param  None
* @retval None
*/
static void QSPI_SetHint(void)
{
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set LCD Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 60, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t*)"QSPI", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t*)"This example tests the modes", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t*)"of read/write access on QSPI memory", CENTER_MODE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
}

/**
* @brief  Fills buffer with user predefined data.
* @param  pBuffer: pointer on the buffer to fill
* @param  uwBufferLenght: size of the buffer to fill
* @param  uwOffset: first value to fill on the buffer
* @retval None
*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
*         ErrorAd: Difference address
* @retval 0: pBuffer identical to pBuffer1
*         1: pBuffer differs from pBuffer1
*/
static uint8_t RMABuffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength, uint32_t *ErrorAd)
{
  (*ErrorAd) = 0;

  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    (*ErrorAd)++;
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
* @}
*/

/**
* @}
*/

