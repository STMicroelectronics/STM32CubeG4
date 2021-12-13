/**
  ******************************************************************************
  * @file    k_storage.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel storage functions
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
#include "stm32g474e_eval.h"
#include "stm32g474e_eval_lcd.h"
#include "stm32g474e_eval_sd.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "k_storage.h"
#include "k_window.h"
#include "string.h"

extern void hx8347d_SetCursor(uint16_t Xpos, uint16_t Ypos);

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Size of a bitmap header (in bytes) */
#define BITMAP_HEADER_SIZE 30

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static FATFS mSDDISK_FatFs;         /* File system object for USB disk logical drive */
static char mSDDISK_Drive[4];       /* USB Host logical drive number */

static uint8_t StorageStatus;
/* Private function prototypes -----------------------------------------------*/
static void kStorage_GetExt(char * pFile, char * pExt);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Storage drives initialization
  * @param  None
  * @retval KMODULE_RETURN status.
  */
STORAGE_RETURN kStorage_Init(void)
{
  /* Link the micro SD disk I/O driver */
  if (FATFS_LinkDriver(&SD_Driver, mSDDISK_Drive)!=0)
  {
    return KSTORAGE_ERROR_LINK;
  }
  /* Register the file system object to the FatFs module */
  if (f_mount(&mSDDISK_FatFs, (TCHAR const*)mSDDISK_Drive, 0) != FR_OK)
  {
    /* FatFs Initialization Error */
    return KSTORAGE_ERROR_MOUNT;
  }
  return KSTORAGE_NOERROR;
}

/**
  * @brief  Storage drives initialization
  * @param  None
  * @retval KMODULE_RETURN status.
  */
STORAGE_RETURN kStorage_DeInit(void)
{
  /* Unlink the RAM disk I/O driver */
  FATFS_UnLinkDriver(mSDDISK_Drive);
  return KSTORAGE_NOERROR;
}

/**
  * @brief  Storage get status
  * @param  None
  * @retval int
  */
uint8_t kStorage_GetStatus(void)
{
  return StorageStatus;
}


/**
  * @brief  Storage get free space
  * @param  None
  * @retval int
  */
uint32_t kStorage_GetFree(void)
{
  uint32_t   fre_clust = 0;
  FATFS *fs ;
  FRESULT res = FR_INT_ERR;

  fs = &mSDDISK_FatFs;
  res = f_getfree("1:", (DWORD *)&fre_clust, &fs);

  if (res == FR_OK)
  {
    return (fre_clust * fs->csize);
  }
  else
  {
    return 0;
  }
}

/**
  * @brief  SD detect callback
  * @param  status   SD card present or not
  * @retval None
  */
void kStorage_SdDetection(uint8_t status)
{
  StorageStatus = status;
  if (status == SD_NOT_PRESENT)
  {
     kWindow_Error("SD card removed\ninsert SD card\nand\nReset the board\n");
     while(1);
  }
}

/**
  * @brief  Return the first file present inside a directory with FileExt
  * @param  DirName  pointer to the Directory name
  * @param  action   Opendir, previous or next files, Closedir
  * @param  FileName pointer to the File name
  * @param  FileExt  pointer to the extension filter if NULL not filter
  * @retval STORAGE_RETURN status
  */
STORAGE_RETURN kStorage_GetDirectoryFiles(const uint8_t *DirName, uint8_t action, uint8_t *FileName, uint8_t *FileExt)
{
  static DIR MyDirectory;
  static uint16_t count = 0;
  uint16_t prev = 0;
  FILINFO MyFileInfo;
  uint8_t ext[4];

  do
  {
    switch(action)
    {
    case KSTORAGE_FINDPREV :
      /* Recherche du prev */
      if (count == 1 ) return KSTORAGE_FIND_NOPREV;
      prev = count-1;
      count = 0;
      f_closedir(&MyDirectory);
      action = KSTORAGE_FINDFIRST;
    break;

    case KSTORAGE_FINDFIRST :
      if (f_opendir(&MyDirectory, (char const *)DirName) != FR_OK)
      {
        return KSTORAGE_FIND_DIRDOESNTEXSIT;
      }
      action = KSTORAGE_FINDNEXT;
    break;

    case KSTORAGE_FINDNEXT :
      do
      {
        /* Get the first file */
        if ((f_readdir(&MyDirectory, &MyFileInfo) != FR_OK) || (MyFileInfo.fname[0] == '\0'))
        {
          return KSTORAGE_FIND_NOFILE;
        }

        /* check the file extension */
        kStorage_GetExt(MyFileInfo.fname, (char *)ext);

        /* part to handle previous */
        if ((prev != 0 ) && ((FileExt == NULL) || (strcmp((char const*)ext, (char const*)FileExt) == 0)))
        {
          count++;
          if (count != prev)
          {
            /* go to next picture */
            ext[0] = '\0';
          }
          else
          {
            count--;
          }
        }
      }
      while((FileExt != NULL) && strcmp((char const*)ext, (char const*)FileExt) != 0);
      count++;
      strcpy((char *)FileName,MyFileInfo.fname);
      action = KSTORAGE_EXIT;
      break;
    case KSTORAGE_FINDCLOSE :
      f_closedir(&MyDirectory);
      action = KSTORAGE_EXIT;
      break;
    }

  }while (action != KSTORAGE_EXIT);

  return KSTORAGE_NOERROR;
}

/**
  * @brief  Check file presence
  * @param  filename pointer to the file name to check
  * @retval STORAGE_RETURN status is KSTORAGE_NOERROR else an error has been detected
  */
STORAGE_RETURN kStorage_FileExist(uint8_t *filename)
{
  FIL F1;

  if (f_open (&F1, (char *)filename, FA_READ) != FR_OK)
  {
    return KSTORAGE_ERROR_OPEN;
  }

  f_close(&F1);
  return KSTORAGE_NOERROR;
}

/**
  * @brief  retrieve info about a file
  * @param  filename pointer to the file name to check
  * @param  fileinfo pointer to the Information about the file
  * @retval STORAGE_RETURN status is KSTORAGE_NOERROR else an error has been detected
  */
STORAGE_RETURN kStorage_GetFileInfo(uint8_t *filename, FILINFO* fileinfo)
{
  if (f_stat((char *)filename, fileinfo) != FR_OK )
  {
    return KSTORAGE_ERROR_OPEN;
  }
  return KSTORAGE_NOERROR;
}

/**
  * @brief  Open a file and use BMP Draw to display the image (limited for 8k images)
  * @param  xpos     x position for the image
  * @param  xpos     y position for the image
  * @param  BmpName  pointer to the file name
  * @retval STORAGE_RETURN status is KSTORAGE_NOERROR else an error has been detected
  */
STORAGE_RETURN kStorage_OpenFileDrawBMP(uint16_t xpos, uint16_t ypos, uint8_t *BmpName)
{
  unsigned int BytesRead;
  FIL F1;
  uint8_t *kstorage_tmpbuffer = NULL;

  if (f_open (&F1, (char *)BmpName, FA_READ) != FR_OK)
  {
    return KSTORAGE_ERROR_OPEN;
  }

  /* Memory allocation to get picture data (limited to 8k*/
  kstorage_tmpbuffer = malloc(sizeof(uint8_t)*(8192+1024));
  if (kstorage_tmpbuffer == NULL)
  {
    f_close(&F1);
    return KSTORAGE_ERROR_MALLOC;
  }

  /* Read the data file content limited to size 8192+1024 */
  if (f_read (&F1,(uint8_t*) kstorage_tmpbuffer, sizeof(uint8_t)*(8192+1024), &BytesRead) != FR_OK)
  {
    f_close(&F1);
    free(kstorage_tmpbuffer);
    return KSTORAGE_ERROR_READ;
  }

  /* Display the image */
  BSP_LCD_DrawBitmap(LCD_INSTANCE, xpos, ypos, kstorage_tmpbuffer);
  free(kstorage_tmpbuffer);
  f_close(&F1);

  return KSTORAGE_NOERROR;
}


/**
  * @brief  Open a file and draw each pixel of the corresponding file
  * @param  xpos    x position for the image
  * @param  xpos    y position for the image
  * @param  BmpName pointer to the file name
  * @retval STORAGE_RETURN status is KSTORAGE_NOERROR else an error has been detected
  */
STORAGE_RETURN kStorage_OpenFileDrawPixel(uint16_t xpos, uint16_t ypos, uint8_t *BmpName)
{
  uint32_t  offset;
  uint32_t  size;
  uint32_t  width;
  uint32_t  height;
  uint32_t  bits_per_pixel;
  uint32_t  bytes_read;
  FIL       file1;
  uint8_t * buffer = (uint8_t *)NULL;
  uint32_t  line_size;

  if (f_open(&file1, (char *)BmpName, FA_READ) != FR_OK)
  {
    return KSTORAGE_ERROR_OPEN;
  }

  /* Memory allocation for the BMP header */
  buffer = malloc(BITMAP_HEADER_SIZE);
  if (buffer == NULL)
  {
    f_close(&file1);
    return KSTORAGE_ERROR_MALLOC;
  }

  /* Read BMP header */
  if (f_read(&file1, (char *)buffer, BITMAP_HEADER_SIZE, (UINT*)&bytes_read) != FR_OK)
  {
    free(buffer);
    f_close(&file1);
    return KSTORAGE_ERROR_READ;
  }

  /* Read bitmap size */
  size  =  *(uint16_t *) (buffer + 2);
  size |= (*(uint16_t *) (buffer + 4)) << 16;

  /* Get bitmap data address offset */
  offset  =  *(uint16_t *) (buffer + 10);
  offset |= (*(uint16_t *) (buffer + 12)) << 16;

  /* Read bitmap width */
  width  =  *(uint16_t *) (buffer + 18);
  width |= (*(uint16_t *) (buffer + 20)) << 16;

  /* Read bitmap height */
  height  =  *(uint16_t *) (buffer + 22);
  height |= (*(uint16_t *) (buffer + 24)) << 16;

  /* Read bit/pixel */
  bits_per_pixel = (*(uint16_t *) (buffer + 28)) / 8;

  line_size = width * bits_per_pixel;
  size = (size - offset);
  xpos = xpos + height - 1;

  /* Synchronize f_read right in front of the image data */
  f_lseek (&file1, offset);

  /* Memory allocation for the BMP data line */
  free(buffer);
  buffer = malloc(line_size);
  if (buffer == NULL)
  {
    return KSTORAGE_ERROR_OPEN;
  }

  do {
    /* read a line */
    if (f_read(&file1, buffer, line_size, (UINT*)&bytes_read) != FR_OK)
    {
      free(buffer);
      f_close(&file1);
      return KSTORAGE_ERROR_READ;
    }

    size -= bytes_read;

    /* Write a line */
    BSP_LCD_FillRGBRect(LCD_INSTANCE, (uint32_t)ypos, (uint32_t)xpos, buffer, width, 1);

    xpos--;
  } while (size != 0);

  /* Exit : free memory, close the file */
  free(buffer);
  f_close(&file1);

  return KSTORAGE_NOERROR;
}


/**
  * @brief  Return file Extension
  * @param  pFile   pointer to the file 
  * @param  pExt    pointer to the file extension
  * @retval None
  */
void kStorage_GetExt(char * pFile, char * pExt)
{
  uint8_t i = 0, j = 0;

  pExt[0] = '\0';

  /* Search beginning of extension */
  while(pFile[i] != '.' && pFile[i] != '\0')
  { i++; };

  if (pFile[i] == '\0') return;
  i++;
  /* Copy extension */
  do {
    pExt[j] = pFile[i];
    j++; i++;
  } while(pFile[i]!= '\0');
  pExt[j] = '\0';
}


