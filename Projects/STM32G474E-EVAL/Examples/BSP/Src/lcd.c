/**
  ******************************************************************************
  * @file    BSP/Src/lcd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use LCD drawing features in the
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
#include <stdio.h>

/** @addtogroup STM32G4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LCD_FEATURES_NUM                6
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Feature = 0;
/* Private function prototypes -----------------------------------------------*/
static void LCD_SetHint(void);
static void LCD_Show_Feature(uint8_t feature);
static void ReadPixelTest(void);
static void BrightnessTest(void);
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  LCD demo
  * @param  None
  * @retval 0 if ok, else value < 0.
  */
int32_t Lcd_demo(void)
{
  int32_t   result = 0;
  uint32_t  Brightness;
  uint32_t  Xsize, Ysize;
  uint32_t  k;
  uint32_t  Orientation[4] = {LCD_ORIENTATION_PORTRAIT, LCD_ORIENTATION_LANDSCAPE, LCD_ORIENTATION_PORTRAIT_ROT180, LCD_ORIENTATION_LANDSCAPE_ROT180};
  char*     Text[4] = {"portrait", "landscape", "portrait with 180 degree rotation", "landscape with 180 degree rotation"};

  Point     triangle1[3] = {{145, 5}, {145, 45}, {185, 45}};
  Point     triangle2[3] = {{190, 45}, {230, 45}, {230, 5}};
  uint32_t  Pixel;

  for (k = 0; k < 4; k++)
  {
    /********************/
    /* TEST WITHOUT GUI */
    /********************/
    printf("\r\nTest with orientation %s\n", Text[k]);

    /* Initialize LCD */
    if (BSP_LCD_Init(0, Orientation[k]) != BSP_ERROR_NONE) result--;

    /* Set the brightness */
    if (BSP_LCD_SetBrightness(0, 50) != BSP_ERROR_COMPONENT_FAILURE) result--;

    /* Get the brightness */
    if (BSP_LCD_GetBrightness(0, &Brightness) != BSP_ERROR_COMPONENT_FAILURE) result--;

    /* Get the display Xsize */
    if (BSP_LCD_GetXSize(0, &Xsize) != BSP_ERROR_NONE) result--;
    if ((Orientation[k] == LCD_ORIENTATION_PORTRAIT) || (Orientation[k] == LCD_ORIENTATION_PORTRAIT_ROT180))
    {
      if (Xsize != 240U) result--;
    }
    else
    {
      if (Xsize != 320U) result--;
    }

    /* Get the display Ysize */
    if (BSP_LCD_GetYSize(0, &Ysize) != BSP_ERROR_NONE) result--;
    if ((Orientation[k] == LCD_ORIENTATION_PORTRAIT) || (Orientation[k] == LCD_ORIENTATION_PORTRAIT_ROT180))
    {
      if (Ysize != 320U) result--;
    }
    else
    {
      if (Ysize != 240U) result--;
    }

    /* Clean display */
    uint32_t i, j;

    for (i = 0; i < Xsize; i++)
    {
      for (j = 0; j < Ysize; j++)
      {
        if (BSP_LCD_WritePixel(0, i, j, LCD_COLOR_RGB565_WHITE) != BSP_ERROR_NONE) result--;
      }
    }

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) result--;

    /* Display a brown rectangle pixel by pixel */
    for (i = 0; i < 60; i++)
    {
      for (j = 0; j < 120; j++)
      {
        if (BSP_LCD_WritePixel(0, i, j, LCD_COLOR_RGB565_DARKGREEN) != BSP_ERROR_NONE) result--;
      }
    }

    /* Get pixel */
    if (BSP_LCD_ReadPixel(0, 20, 21, &Pixel) != BSP_ERROR_NONE) result--;
    if (Pixel != LCD_COLOR_RGB565_DARKGREEN) result--;

    /* Display a red rectangle */
    if (BSP_LCD_FillRect(0, 80, 20, 80, 60, LCD_COLOR_RGB565_RED) != BSP_ERROR_NONE) result--;

    /* Display a green horizontal line */
    if (BSP_LCD_DrawHLine(0, 80, 120, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a green vertical line */
    if (BSP_LCD_DrawVLine(0, 80, 120, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a green horizontal line */
    if (BSP_LCD_DrawHLine(0, 80, 210, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a green vertical line */
    if (BSP_LCD_DrawVLine(0, 170, 120, 90, LCD_COLOR_RGB565_GREEN) != BSP_ERROR_NONE) result--;

    /* Display a bitmap */
    if (BSP_LCD_DrawBitmap(0, 85, 135, (uint8_t *)stlogo) != BSP_ERROR_NONE) result--;

    HAL_Delay(1000);

    /* Set the display off */
    if (BSP_LCD_DisplayOff(0) != BSP_ERROR_NONE) result--;

    HAL_Delay(1000);

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) result--;

    /* Check result */
    if (CheckResult() != 0U) result--;

    /* De-initialize LCD */
    if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE) result--;

    HAL_Delay(1000);

    /*****************/
    /* TEST WITH GUI */
    /*****************/
    printf("\r\nTest with orientation %s and GUI\n", Text[k]);

    /* Initialize LCD */
    if (BSP_LCD_Init(0, Orientation[k]) != BSP_ERROR_NONE) result--;

    /* Set GUI functions */
     UTIL_LCD_SetFuncDriver(&LCD_Driver); /* SetFunc before setting device */
     UTIL_LCD_SetDevice(0);/* SetDevice after funcDriver is set */

    /* Clear screen */
    BSP_LCD_Clear(0, LCD_COLOR_RGB565_WHITE);

    /* Set the display on */
    if (BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE) result--;

    /* Draw a black pixel */
    UTIL_LCD_SetPixel(25, 25, UTIL_LCD_COLOR_BLACK);

    /* Draw a yellow circle */
    UTIL_LCD_DrawCircle(25, 25, 20, UTIL_LCD_COLOR_YELLOW);

    /* Fill a magenta circle */
    UTIL_LCD_FillCircle(70, 25, 20, UTIL_LCD_COLOR_MAGENTA);

    /* Draw a blue vertical line */
    UTIL_LCD_DrawVLine(95, 5, 40, UTIL_LCD_COLOR_BLUE);

    /* Draw a red horizontal line */
    UTIL_LCD_DrawHLine(100, 25, 40, UTIL_LCD_COLOR_RED);

    /* Draw a green triangle */
    UTIL_LCD_DrawPolygon(triangle1, 3, UTIL_LCD_COLOR_GREEN);

    /* Fill a cyan triangle */
    UTIL_LCD_FillPolygon(triangle2, 3, UTIL_LCD_COLOR_CYAN);

    /* Draw a gray rectangle */
    UTIL_LCD_DrawRect(5, 50, 40, 20, UTIL_LCD_COLOR_GRAY);

    /* Fill a brown rectangle */
    UTIL_LCD_FillRect(50, 50, 40, 20, UTIL_LCD_COLOR_BROWN);

    /* Draw a orange ellipse */
    UTIL_LCD_DrawEllipse(115, 60, 20, 10, UTIL_LCD_COLOR_ORANGE);

    /* Fill a dark blue ellipse */
    UTIL_LCD_FillEllipse(170, 60, 20, 10, UTIL_LCD_COLOR_DARKBLUE);

    /* Display text */
    UTIL_LCD_SetFont(&Font24);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAtLine(3, (uint8_t*)"Test of LCD");
    UTIL_LCD_SetFont(&Font12);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ORANGE);
    UTIL_LCD_DisplayStringAt(0, 112, (uint8_t*)"Test of LCD", CENTER_MODE);

    /* Draw bitmap */
    UTIL_LCD_DrawBitmap(5, 130, (uint8_t *)stlogo);

    /* Check result */
    if (CheckResult() != 0U) result--;

    /* De-initialize LCD */
    if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE) result--;
  }

  BSP_LCD_Init(0, Orientation[1]);
  return result;
}

/**
  * @brief  LCD demo
  * @param  None
  * @retval None
  */
void LCD_demo (void)
{
  LCD_SetHint();
  LCD_Feature = 0;
  LCD_Show_Feature (LCD_Feature);

  while (1)
  {

    if(CheckForUserInput() > 0)
    {
      if(++LCD_Feature < LCD_FEATURES_NUM)
      {
       LCD_Show_Feature (LCD_Feature);
      }
      else
      {
        return;
      }
    }
    HAL_Delay(100);
  }
}

/**
  * @brief  Display LCD demo hint
  * @param  None
  * @retval None
  */
static void LCD_SetHint(void)
{
  uint32_t x_size, y_size;
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  /* Set LCD Demo description */

  BSP_LCD_FillRect(0, 0, 0, x_size, 80, UTIL_LCD_COLOR_BLUE );

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"LCD", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"This example shows the different", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"LCD Features, use Tamper push-button to display", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"next page", CENTER_MODE);

   /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 90, x_size - 20, y_size- 100, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 91, x_size - 22, y_size- 102, UTIL_LCD_COLOR_BLUE);
 }

/**
  * @brief  Show LCD Features
  * @param  feature : feature index
  * @retval None
  */
static void LCD_Show_Feature(uint8_t feature)
{
  Point Points[]= {{220, 220}, {280, 180}, {320, 180}, {370, 220}, {370, 260}, {320, 310}, {280, 310}, {220, 260}};
  Point Points2[3];
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  Points2[0].X = x_size - 80;
  Points2[0].Y = 150;
  Points2[1].X = x_size - 20;
  Points2[1].Y = 150;
  Points2[2].X = x_size - 20;
  Points2[2].Y = 200;

  BSP_LCD_FillRect(0, 12, 92, x_size - 24, y_size- 104, LCD_COLOR_RGB565_WHITE );
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLACK);

  switch (feature)
  {
  case 0:
    /* Text Feature */
    UTIL_LCD_DisplayStringAt(14, 100, (uint8_t *)"Left aligned Text", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(0, 115, (uint8_t *)"Center aligned Text", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(14, 130, (uint8_t*)"Right aligned Text", RIGHT_MODE);
    UTIL_LCD_SetFont(&Font24);
    UTIL_LCD_DisplayStringAt(14, 180, (uint8_t *)"Font24", LEFT_MODE);
    UTIL_LCD_SetFont(&Font20);
    UTIL_LCD_DisplayStringAt(x_size/2 -20, 180, (uint8_t *)"Font20", LEFT_MODE);
    UTIL_LCD_SetFont(&Font16);
    UTIL_LCD_DisplayStringAt(x_size - 80, 184, (uint8_t *)"Font16", LEFT_MODE);
    HAL_Delay(2000);
    break;

  case 1:

    /* Draw misc. Shapes */
    UTIL_LCD_DrawRect ( 20, 100, 60 , 40, UTIL_LCD_COLOR_BLACK);
    BSP_LCD_FillRect(0, 100, 100, 60 , 40, LCD_COLOR_RGB565_BLACK);

//    UTIL_LCD_DrawCircle( x_size - 120, 120, 20);
//    UTIL_LCD_FillCircle( x_size - 40, 120, 20, UTIL_LCD_COLOR_GRAY);

    UTIL_LCD_FillPolygon(Points, 8, UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DrawEllipse(130, 170, 30, 20, UTIL_LCD_COLOR_RED);
    UTIL_LCD_FillEllipse(200, 170, 30, 20, UTIL_LCD_COLOR_RED);

    BSP_LCD_DrawHLine(0, 20, y_size - 30, x_size / 5, LCD_COLOR_RGB565_BLACK);
    UTIL_LCD_DrawLine ( 100, y_size - 20, 230, y_size- 50, UTIL_LCD_COLOR_BLACK);
    UTIL_LCD_DrawLine ( 100, y_size- 50, 230, y_size- 20, UTIL_LCD_COLOR_BLACK);

    UTIL_LCD_DrawPolygon(Points2, 3, UTIL_LCD_COLOR_YELLOW);
    HAL_Delay(2000);
    break;

  case 2:
    /* Draw Bitmap */
    BSP_LCD_DrawBitmap(0, 20, 100, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size/2 - 40, 100, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size-100, 100, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, 20, y_size- 80, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size/2 - 40, y_size- 80, (uint8_t *)stlogo);
    HAL_Delay(500);

    BSP_LCD_DrawBitmap(0, x_size-100, y_size- 80, (uint8_t *)stlogo);
    HAL_Delay(500);
    break;

    case 3:
    /* Set LCD Brightness */
     BrightnessTest();
    break;

    case 4:
    /* LCD Read Pixel */
     ReadPixelTest();
    break;
  case 5:

    //LCD_SetFuncDriver(&LCD_Driver);
    UTIL_LCD_SetFont(&Font20);
    /* Clear the LCD */
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

    BSP_LCD_FillRect(0,0, 0, x_size, 80, UTIL_LCD_COLOR_WHITE);

    /* Set the LCD Text Color */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: LANDSCAPE", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB565", CENTER_MODE);
    HAL_Delay(2000);
    //LCD_SetFuncDriver(&LCD_Driver);
    UTIL_LCD_SetFont(&Font20);
    /* Clear the LCD */
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

#if 0
    BSP_LCD_FillRect(0, 0, 0, x_size, 80, LCD_COLOR_RGB565_WHITE);
    /* Set the LCD Text Color */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: PORTRAIT", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB565", CENTER_MODE);
    HAL_Delay(2000);

    BSP_LCD_DeInit(0);
    BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT);
    //LCD_SetFuncDriver(&LCD_Driver);
    UTIL_LCD_SetFont(&LCD_DEFAULT_FONT);
    /* Clear the LCD */
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

    BSP_LCD_FillRect(0, 0, 0, x_size, 80, LCD_COLOR_RGD565_WHITE);
    /* Set the LCD Text Color */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: PORTRAIT", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB888", CENTER_MODE);
    HAL_Delay(2000);
#endif
    BSP_LCD_DeInit(0);
    BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
    //LCD_SetFuncDriver(&LCD_Driver);
    UTIL_LCD_SetFont(&Font20);
    /* Clear the LCD */
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLUE);
    BSP_LCD_GetXSize(0, &x_size);

    BSP_LCD_FillRect(0, 0, 0, x_size, 80, LCD_COLOR_RGB565_WHITE);
    /* Set the LCD Text Color */
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(0, y_size/2, (uint8_t *)"Orientation: LANDSCAPE", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)"Pixel Format: RGB888", CENTER_MODE);
    HAL_Delay(2000);

    break;
  }
}

/**
  * @brief  LCD Read Pixel test
  * @param  None
  * @retval None
  */
static void ReadPixelTest(void)
{
  uint16_t i, j, k;
  uint32_t x_size, y_size, read_pixel;
  uint32_t colors[23] = {UTIL_LCD_COLOR_BLUE, UTIL_LCD_COLOR_GREEN, UTIL_LCD_COLOR_RED, UTIL_LCD_COLOR_CYAN, UTIL_LCD_COLOR_MAGENTA, UTIL_LCD_COLOR_YELLOW,
  UTIL_LCD_COLOR_LIGHTBLUE, UTIL_LCD_COLOR_LIGHTGREEN, UTIL_LCD_COLOR_LIGHTRED, UTIL_LCD_COLOR_LIGHTMAGENTA,
  UTIL_LCD_COLOR_LIGHTYELLOW, UTIL_LCD_COLOR_DARKBLUE, UTIL_LCD_COLOR_DARKGREEN, UTIL_LCD_COLOR_DARKRED, UTIL_LCD_COLOR_DARKCYAN,
  UTIL_LCD_COLOR_DARKMAGENTA, UTIL_LCD_COLOR_DARKYELLOW, UTIL_LCD_COLOR_LIGHTGRAY, UTIL_LCD_COLOR_GRAY, UTIL_LCD_COLOR_DARKGRAY,
  UTIL_LCD_COLOR_BLACK, UTIL_LCD_COLOR_BROWN, UTIL_LCD_COLOR_ORANGE };


  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

#if (LCD_HX8347D == 1)
  for(k = 0; k < 23; k++)
  {
    UTIL_LCD_Clear(colors[k]);

    BSP_LCD_ReadPixel(0, i++, j++, &read_pixel);
    if(read_pixel != colors[k])
    {
          BSP_LED_On(LED_RED);
    }
  }
#else
  for(k = 0; k < 23; k++)
  {
    UTIL_LCD_Clear(colors[k]);
    for(j = 0; j < y_size; j++)
    {
      for(i = 0; i < x_size; i++)
      {
        BSP_LCD_ReadPixel(0, i,j, &read_pixel);
        if(read_pixel != colors[k])
        {
          BSP_LED_On(LED_RED);
        }
      }
    }
  }
#endif /* (LCD_HX8347D == 1)*/
}

/**
  * @brief  Show LCD Brightness Feature
  * @param  None
  * @retval None
  */
static void BrightnessTest(void)
{
  uint8_t counter = 100;
  uint8_t text[30];
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &y_size);

#if (LCD_HX8347D == 1)
    /* brightness control is not supported by controller */
    BSP_LCD_SetBrightness(0, counter);
    sprintf((char*)text," Brightness = %d ",counter);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)text, CENTER_MODE);
    HAL_Delay(50);
#else
  while(counter > 0)
  {
    BSP_LCD_SetBrightness(0, counter);
    sprintf((char*)text," Brightness = %d ",counter);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)text, CENTER_MODE);
    counter = counter - 10/*counter--*/;
    HAL_Delay(50);
  }
  while(counter <= 100)
  {
    BSP_LCD_SetBrightness(0, counter);
    sprintf((char*)text," Brightness = %d ",counter);
    UTIL_LCD_DisplayStringAt(0, y_size/2 + 45, (uint8_t *)text, CENTER_MODE);
    counter = counter + 10/*counter++*/;
    HAL_Delay(50);
  }
#endif /* LCD_HX8347D*/
}

/**
  * @}
  */

/**
  * @}
  */

