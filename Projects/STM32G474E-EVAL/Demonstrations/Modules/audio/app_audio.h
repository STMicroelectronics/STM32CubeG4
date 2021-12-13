/**
  ******************************************************************************
  * @file    app_audio.h
  * @author  MCD Application Team
  * @brief   Audio application interface
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
#undef GLOBAL
#ifdef __APP_AUDIO_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "ff.h"
#include "k_module.h"

/* Exported types ------------------------------------------------------------*/
/* WAV header structure */
typedef struct
{
  char       RIFF[4];          /* 0 */
  uint32_t   ChunkSize;        /* 4 */
  char       WAVE[4];          /* 8 */
  char       fmt[4];           /* 12 */
  uint32_t   SubChunk1Size;    /* 16*/
  uint16_t   AudioFormat;      /* 20 */
  uint16_t   NbrChannels;      /* 22 */
  uint32_t   SampleRate;       /* 24 */
  uint32_t   ByteRate;         /* 28 */
  uint16_t   BlockAlign;       /* 32 */
  uint16_t   BitPerSample;     /* 34 */
} WAV_HeaderTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Size of audio half buffer in byte */
#define BUFFER_HALF_SIZE_BYTE   (_MAX_SS*8)
#define BUFFER_HALF_SIZE_WORD   BUFFER_HALF_SIZE_BYTE/4

/* Size of the header of a WAV (RIFF) file (36 bytes long) */
#define WAV_HEADER_SIZE 36

/* Offset of the "fmt" sub-chunk in the wave header */
#define FMT_CHK_POS 12

/* Sub-chunk id length (in bbytes) */
#define SUB_CHUNK_ID_LENGHT 4

/* Record file name */
#define REC_FILE_NAME           "USER/rec.wav"

/* Definition of DMA status */
typedef enum {
STARTED=0,
HALF_COMPLETE,
COMPLETE,
} DMA_StatusTypedef;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
GLOBAL const K_ModuleItem_Typedef ModuleAudio;

/* WAVE file descriptor */
GLOBAL FIL WaveFile;

/* Audio samples buffer */
GLOBAL uint32_t Buffer[BUFFER_HALF_SIZE_WORD * 2];

/* DMA transfer complete flag */
GLOBAL __IO DMA_StatusTypedef DMA_AudioInComplete;
GLOBAL __IO DMA_StatusTypedef DMA_AudioOutComplete;

/* Progress bar handle */
GLOBAL void * hProgressBar;

/* Exported functions --------------------------------------------------------*/


