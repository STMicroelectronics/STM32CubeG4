/**
  ******************************************************************************
  * @file    data_struct_tlv.h
  * @author  MCD Application Team
  * @brief   Include file used in TLV structures and/or strings handling.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include <stdint.h>

#ifndef DATA_STRUCT_TLV_H_
#define DATA_STRUCT_TLV_H_

#define TLV_SOF         0xFDu
#define TLV_EOF         0xA5u
#define TLV_SIZE_MAX    256u

#define TLV_TAG_POSITION          4u /* Byte position of TAG                    */
#define TLV_LENGTH_HIGH_POSITION  5u /* Byte position of LENGTH HIGH            */
#define TLV_LENGTH_LOW_POSITION   6u /* Byte position of LENGTH LOW             */
#define TLV_VALUE_POSITION        7u /* Byte position of VALUE                  */
#define TLV_HEADER_SIZE           3u /* Size of TLV header (TAG(1) + LENGTH(2)  */
#define TLV_SOF_SIZE              4u /* TLV_SOF * 4                             */
#define TLV_EOF_SIZE              4u /* TLV_EOF * 4                             */

/**
 * @struct    TLV_ToSend_DATA
 * @brief     Structure used when forming a TLV which is about to be send
 */
typedef struct
{
  uint8_t*  data;      /*!< The TLV instruction    */
  uint16_t  maxSize;                /*!< Max size of the instruction        */
  uint16_t  EOFposition;            /*!< Position of the EOF in data        */
  uint16_t  SizePosition;            /*!< Position of the Size tag        */
} TLV_ToSend_Data;

/**
 * @struct    TLV_Received_DATA
 * @brief     Structure used when interpreting a received TLV
 */
typedef struct
{
  uint8_t*  data;      /*!< The TLV instruction    */
  uint16_t        cursor;                 /*!< Read position                      */
} TLV_Received_Data;


uint16_t                TLV_get_string_length(const uint8_t* string);

uint8_t                 TLV_init_encode(TLV_ToSend_Data *ToSendTLV, uint8_t tag, uint16_t sizeMax, uint8_t *ptr);
uint16_t                TLV_add(TLV_ToSend_Data *ToSendTLV, uint8_t tag, uint16_t size, const uint8_t *value);
uint16_t                TLV_addValue(TLV_ToSend_Data *ToSendTLV, const uint8_t *Value, uint16_t Size);
void                    TLV_UpdateSizeTag(TLV_ToSend_Data *ToSendTLV, uint16_t Size);
void                    TLV_deinit_encode(TLV_ToSend_Data *ToSendTLV);
uint8_t                 TLV_init_decode(TLV_Received_Data *ToProcessTLV, uint8_t *receivedString);
uint8_t                 TLV_get(TLV_Received_Data *ToProcessTLV, uint8_t *tag, uint16_t *length, uint8_t **value);
void                    TLV_deinit_decode(TLV_Received_Data *ToProcessTLV);

#endif /* DATA_STRUCT_TLV_H_ */

