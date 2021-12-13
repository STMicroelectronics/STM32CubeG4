/**
  ******************************************************************************
  * @file    k_storage.h
  * @author  MCD Application Team
  * @brief   Header for k_storage.c file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __K_STORAGE_H
#define __K_STORAGE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "ff.h"
   
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
typedef enum {
  KSTORAGE_NOERROR,
  KSTORAGE_ERROR_LINK,
  KSTORAGE_ERROR_MOUNT,
  KSTORAGE_ERROR_OPEN,
  KSTORAGE_ERROR_READ,
  KSTORAGE_ERROR_WRITE,
  KSTORAGE_ERROR_CLOSE,
  KSTORAGE_FIND_DIRDOESNTEXSIT,
  KSTORAGE_FIND_NOFILE,
  KSTORAGE_FIND_NOPREV,
  KSTORAGE_ERROR_MALLOC
} STORAGE_RETURN;

enum {
  KSTORAGE_FINDFIRST,
  KSTORAGE_FINDNEXT,
  KSTORAGE_FINDPREV,
  KSTORAGE_FINDCLOSE,
  KSTORAGE_EXIT,
};
/* Exported functions ------------------------------------------------------- */
STORAGE_RETURN kStorage_Init(void);
STORAGE_RETURN kStorage_DeInit(void);
STORAGE_RETURN kStorage_OpenFileDrawBMP(uint16_t xpos, uint16_t ypos, uint8_t *BmpName);
STORAGE_RETURN kStorage_OpenFileDrawPixel(uint16_t xpos, uint16_t ypos, uint8_t *BmpName);
STORAGE_RETURN kStorage_FileExist(uint8_t *filename);
STORAGE_RETURN kStorage_GetFileInfo(uint8_t *filename,FILINFO* fileinfo);
STORAGE_RETURN kStorage_GetDirectoryFiles(const uint8_t *DirName, uint8_t action, uint8_t *FileName, uint8_t *FileExt);

uint8_t  kStorage_GetStatus (void);
void     kStorage_SdDetection(uint8_t status);
uint32_t kStorage_GetCapacity (void);
uint32_t kStorage_GetFree (void);

#ifdef __cplusplus
}
#endif

#endif /*__K_STORAGE_H */


