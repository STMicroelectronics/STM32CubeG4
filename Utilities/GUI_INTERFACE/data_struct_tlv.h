/**
  ******************************************************************************
  * @file    data_struct_tlv.h
  * @author  MCD Application Team
  * @brief   Include file used in TLV structures and/or strings handling.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef DATA_STRUCT_TLV_H_
#define DATA_STRUCT_TLV_H_

#include <stdint.h>

#define TLV_SOF         0xFDU
#define TLV_EOF         0xA5U
#define TLV_SIZE_MAX    256U

#define TLV_TAG_POSITION          4U /* Byte position of TAG                    */
#define TLV_LENGTH_HIGH_POSITION  5U /* Byte position of LENGTH HIGH            */
#define TLV_LENGTH_LOW_POSITION   6U /* Byte position of LENGTH LOW             */
#define TLV_VALUE_POSITION        7U /* Byte position of VALUE                  */
#define TLV_HEADER_SIZE           3U /* Size of TLV header (TAG(1) + LENGTH(2)  */
#define TLV_SOF_SIZE              4U /* TLV_SOF * 4                             */
#define TLV_EOF_SIZE              4U /* TLV_EOF * 4                             */

/**
  * @struct    TLV_ToSend_Data_t
  * @brief     Structure used when forming a TLV which is about to be send
  */
typedef struct
{
  uint8_t  *data;           /*!< The TLV instruction            */
  uint16_t  maxSize;        /*!< Max size of the instruction    */
  uint16_t  EOFposition;    /*!< Position of the EOF in data    */
  uint16_t  SizePosition;   /*!< Position of the Size tag       */
} TLV_ToSend_Data_t;

/**
  * @struct    TLV_Received_Data_t
  * @brief     Structure used when interpreting a received TLV
  */
typedef struct
{
  uint8_t  *data;           /*!< The TLV instruction            */
  uint16_t  cursor;         /*!< Read position                  */
} TLV_Received_Data_t;


uint16_t                TLV_get_string_length(const uint8_t *string);

uint8_t                 TLV_init_encode(TLV_ToSend_Data_t *ToSendTLV, uint8_t tag, uint16_t sizeMax, uint8_t *ptr);
uint16_t                TLV_add(TLV_ToSend_Data_t *ToSendTLV, uint8_t tag, uint16_t size, const uint8_t *value);
uint16_t                TLV_addValue(TLV_ToSend_Data_t *ToSendTLV, const uint8_t *Value, uint16_t Size);
void                    TLV_UpdateSizeTag(TLV_ToSend_Data_t *ToSendTLV, uint16_t Size);
void                    TLV_deinit_encode(TLV_ToSend_Data_t *ToSendTLV);
uint8_t                 TLV_init_decode(TLV_Received_Data_t *ToProcessTLV, uint8_t *receivedString);
uint8_t                 TLV_get(TLV_Received_Data_t *ToProcessTLV, uint8_t *tag, uint16_t *length, uint8_t **value);
void                    TLV_deinit_decode(TLV_Received_Data_t *ToProcessTLV);

#endif /* DATA_STRUCT_TLV_H_ */

