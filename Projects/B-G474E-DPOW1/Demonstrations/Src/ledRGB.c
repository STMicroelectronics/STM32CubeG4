/**
  ******************************************************************************
  * @file    Demonstrations/Src/ledRGB.c
  * @author  MCD Application Team
  * @brief   RGB led color demonstration
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
#define __LEDCOLOR_C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ledRGB.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
  PHASE_RISING = 0,
  PHASE_FALLING,
  PHASE_OFF,
  MAX_PHASE
}LED_PHASE_TypeDef;

/* Private define ------------------------------------------------------------*/

/* Color definition */
#define  RED   0
#define  GREEN 1
#define  BLUE  2

#define COLOR_STEP                 1
#define GLOBAL_BRIGHTNESS_STEP 1

/* PWM frequency 250kHz (4us) with System_clock =170MHz and with Prescaler multiply by 32  */
#define PERIOD ((170000 * 32 ) / 250)

/* Burst period 400Hz. 250000/400 = 625 pulses at 250kHz */
#define BURST_PERIOD   (250000/400)

/* Maximum led current limitation. Same value for all 3 RBG leds. Unit: mA */
#define MAX_LED_CURRENT        100

/* Sense resistor value. Same value for all 3 RBG leds . Unit: Ohm */
#define SENSE_RESISTOR           3

/* Maximum sense voltage. Same value for all 3 RBG leds. Unit mV*/
#define MAX_SENSE_VOLTAGE      (MAX_LED_CURRENT * SENSE_RESISTOR)

/*****************************************************************/
/* Defines for DAC saw tooth generation to limit leds current    */
/*****************************************************************/
/* DAC sawtooth range in mV . It correspond to slope rate 100 mA / 4us  */
#define DAC_SAWTOOTH_RANGE   (100 * SENSE_RESISTOR)

/* Current should hit sawtooth slope in the middle of the slope. Thus sawtooth peak is current limit + held slope range */
#define MAX_DAC_SAW_PEAK     (MAX_SENSE_VOLTAGE + (DAC_SAWTOOTH_RANGE / 2))

/* Fast DAC supports up to 15 Msamples per seconds -> 15MHz/250kHz = 60steps */
#define FAST_DAC_STEP_NB     60

/* sawtooth step parameter is 12.4 bit format (12 bits integer part, 4 bits fractional part), thus value is shifted by 4 bits */
#define FAST_DAC_STEP        (((DAC_SAWTOOTH_RANGE * 4096) << 4) / (3300 * FAST_DAC_STEP_NB))

/* Slow DAC supports up to 1Msamples per seconds whatever conditions.    */
/* but in the present circumstances it can be extended.                  */
/* For RGB led demo due to low load on DAC we use 3Msamples per seconds */
/* -> 3MHz/250kHz = 12steps */
#define SLOW_DAC_STEP_NB     12

/* sawtooth step parameter is 12.4 bit format (12 bits integer part, 4 bits fractional part), thus value is shifted by 4 bits */
#define SLOW_DAC_STEP        (((DAC_SAWTOOTH_RANGE * 4096) << 4) / (3300 * SLOW_DAC_STEP_NB))  /* sawtooth step parameter is 12.4 bit format (12 bits integer part, 4 bits fractional part), thus value is shifted by 4 bits */

/* Threshold for global brightness of white led demo.   */
/* Up to threshold, sawtooth level will increase.       */
/* Above threshold, sawtooth level stay at its maximum  */
#define BRIGHTNESS_THRESHOLD 200

/* comment this line to disable debug: output of DAC on GPIO */
/* Warning: PB1 used for debug of red DAC (through OPAMP) is already used for Orange led */
#define DEBUG_FAST_DAC

/* Uncomment this line to enable debug of slow DAC (Blue) */
/* warning when this switch is activated, the output of slow DAC is enable,  */
/* and this impact waveform which becomes flattened  */
/* Also when this switch is enable, the DAC output buffer is also enable, */
/* and this degrade output signal: specially at low voltage rising edge od sawtooth is delay conpare to HRTIM reset */
//#define DEBUG_SLOW_DAC

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static COMP_HandleTypeDef CompRedHandle;
static COMP_HandleTypeDef CompGreenHandle;
static COMP_HandleTypeDef CompBlueHandle;

static DAC_HandleTypeDef  DacRedHandle;
static DAC_HandleTypeDef  DacGreenHandle;
static DAC_HandleTypeDef  DacBlueHandle;

static OPAMP_HandleTypeDef OpampRedHandle;
static OPAMP_HandleTypeDef OpampGreenHandle;

static LED_PHASE_TypeDef PhaseRGB[3];

static __IO int32_t PerMilleRGB[3];   // Range 0 to 1000

static __IO uint32_t  MaxVoltageSenseRGB[3];

static const uint32_t hrtim_output[3] = {HRTIM_OUTPUT_TF1, HRTIM_OUTPUT_TE1, HRTIM_OUTPUT_TA1};

static int32_t GlobalBrigthness_1000;  // Range [0 .. 1000]

/* Due to color sensitivity, each color must be weighted */
static uint32_t RedWeight;
static uint32_t GreenWeight;
static uint32_t BlueWeight;

/* Gamma correction table: range 0 .. 1000. Gamma factor = 2.2           */
/* GammaCorrection1000[x] = INT((1000* (x/1000)^(2.2)) + 0.5)            */
/* the addition of 0.5 is to round up when fractional part is above 0.5  */
const uint16_t GammaCorrection1000[1001] = // Range [0 .. 1000]
{
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,
  2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,   4,
  4,   4,   4,   4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,   6,   6,   6,
  6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,   8,   8,   9,   9,   9,   9,   9,
  9,  10,  10,  10,  10,  10,  10,  11,  11,  11,  11,  11,  12,  12,  12,  12,  12,  13,  13,  13,
 13,  13,  14,  14,  14,  14,  15,  15,  15,  15,  15,  16,  16,  16,  16,  17,  17,  17,  17,  18,
 18,  18,  18,  18,  19,  19,  19,  19,  20,  20,  20,  21,  21,  21,  21,  22,  22,  22,  22,  23,
 23,  23,  24,  24,  24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27,  27,  28,  28,  28,  29,
 29,  29,  30,  30,  30,  31,  31,  31,  32,  32,  32,  33,  33,  33,  34,  34,  34,  35,  35,  35,
 36,  36,  36,  37,  37,  38,  38,  38,  39,  39,  39,  40,  40,  41,  41,  41,  42,  42,  43,  43,
 43,  44,  44,  44,  45,  45,  46,  46,  47,  47,  47,  48,  48,  49,  49,  49,  50,  50,  51,  51,
 52,  52,  53,  53,  53,  54,  54,  55,  55,  56,  56,  57,  57,  57,  58,  58,  59,  59,  60,  60,
 61,  61,  62,  62,  63,  63,  64,  64,  65,  65,  66,  66,  67,  67,  68,  68,  69,  69,  70,  70,
 71,  71,  72,  72,  73,  73,  74,  74,  75,  75,  76,  77,  77,  78,  78,  79,  79,  80,  80,  81,
 82,  82,  83,  83,  84,  84,  85,  86,  86,  87,  87,  88,  88,  89,  90,  90,  91,  91,  92,  93,
 93,  94,  94,  95,  96,  96,  97,  97,  98,  99,  99, 100, 101, 101, 102, 102, 103, 104, 104, 105,
106, 106, 107, 108, 108, 109, 110, 110, 111, 112, 112, 113, 114, 114, 115, 116, 116, 117, 118, 118,
119, 120, 120, 121, 122, 122, 123, 124, 125, 125, 126, 127, 127, 128, 129, 130, 130, 131, 132, 132,
133, 134, 135, 135, 136, 137, 138, 138, 139, 140, 141, 141, 142, 143, 144, 144, 145, 146, 147, 148,
148, 149, 150, 151, 151, 152, 153, 154, 155, 155, 156, 157, 158, 159, 159, 160, 161, 162, 163, 163,
164, 165, 166, 167, 168, 168, 169, 170, 171, 172, 173, 173, 174, 175, 176, 177, 178, 179, 179, 180,
181, 182, 183, 184, 185, 186, 186, 187, 188, 189, 190, 191, 192, 193, 194, 194, 195, 196, 197, 198,
199, 200, 201, 202, 203, 204, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217,
218, 219, 220, 221, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236,
237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 252, 253, 254, 255, 256, 257,
258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 271, 272, 273, 274, 275, 276, 277, 278,
279, 280, 281, 283, 284, 285, 286, 287, 288, 289, 290, 291, 293, 294, 295, 296, 297, 298, 299, 301,
302, 303, 304, 305, 306, 307, 309, 310, 311, 312, 313, 314, 316, 317, 318, 319, 320, 321, 323, 324,
325, 326, 327, 329, 330, 331, 332, 333, 335, 336, 337, 338, 340, 341, 342, 343, 344, 346, 347, 348,
349, 351, 352, 353, 354, 356, 357, 358, 359, 361, 362, 363, 364, 366, 367, 368, 369, 371, 372, 373,
375, 376, 377, 378, 380, 381, 382, 384, 385, 386, 388, 389, 390, 392, 393, 394, 396, 397, 398, 400,
401, 402, 404, 405, 406, 408, 409, 410, 412, 413, 414, 416, 417, 418, 420, 421, 423, 424, 425, 427,
428, 429, 431, 432, 434, 435, 436, 438, 439, 441, 442, 443, 445, 446, 448, 449, 451, 452, 453, 455,
456, 458, 459, 461, 462, 463, 465, 466, 468, 469, 471, 472, 474, 475, 477, 478, 480, 481, 482, 484,
485, 487, 488, 490, 491, 493, 494, 496, 497, 499, 500, 502, 503, 505, 506, 508, 509, 511, 513, 514,
516, 517, 519, 520, 522, 523, 525, 526, 528, 529, 531, 533, 534, 536, 537, 539, 540, 542, 544, 545,
547, 548, 550, 552, 553, 555, 556, 558, 559, 561, 563, 564, 566, 568, 569, 571, 572, 574, 576, 577,
579, 581, 582, 584, 585, 587, 589, 590, 592, 594, 595, 597, 599, 600, 602, 604, 605, 607, 609, 610,
612, 614, 615, 617, 619, 621, 622, 624, 626, 627, 629, 631, 632, 634, 636, 638, 639, 641, 643, 645,
646, 648, 650, 651, 653, 655, 657, 658, 660, 662, 664, 665, 667, 669, 671, 673, 674, 676, 678, 680,
681, 683, 685, 687, 689, 690, 692, 694, 696, 698, 699, 701, 703, 705, 707, 708, 710, 712, 714, 716,
718, 719, 721, 723, 725, 727, 729, 731, 732, 734, 736, 738, 740, 742, 744, 745, 747, 749, 751, 753,
755, 757, 759, 761, 762, 764, 766, 768, 770, 772, 774, 776, 778, 780, 782, 783, 785, 787, 789, 791,
793, 795, 797, 799, 801, 803, 805, 807, 809, 811, 813, 815, 817, 819, 821, 822, 824, 826, 828, 830,
832, 834, 836, 838, 840, 842, 844, 846, 848, 850, 852, 854, 856, 858, 861, 863, 865, 867, 869, 871,
873, 875, 877, 879, 881, 883, 885, 887, 889, 891, 893, 895, 897, 900, 902, 904, 906, 908, 910, 912,
914, 916, 918, 920, 923, 925, 927, 929, 931, 933, 935, 937, 939, 942, 944, 946, 948, 950, 952, 954,
957, 959, 961, 963, 965, 967, 969, 972, 974, 976, 978, 980, 982, 985, 987, 989, 991, 993, 996, 998,
1000
};

/* Private function prototypes -----------------------------------------------*/
void RGBLed_Init(void);
void RGBLed_DeInit(void);

static void DAC_Red_Config(void);
static void DAC_Green_Config(void);
static void DAC_Blue_Config(void);

static void COMP_Red_Config(void);
static void COMP_Green_Config(void);
static void COMP_Blue_Config(void);

static void HRTIM_Config(void);

static void DAC_Red_DeConfig(void);
static void DAC_Green_DeConfig(void);
static void DAC_Blue_DeConfig(void);

static void COMP_Red_DeConfig(void);
static void COMP_Green_DeConfig(void);
static void COMP_Blue_DeConfig(void);
static void HRTIM_DeConfig(void);

#ifdef DEBUG_FAST_DAC
static void OPAMP_Red_Config(void);
static void OPAMP_Green_Config(void);
static void OPAMP_Red_DeConfig(void);
static void OPAMP_Green_DeConfig(void);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  RGB led color demonstration
  *********************************************************************
  *         Generate following pattern for color part per mille
  *
  *          R  \       /\
  *              \     /  \
  *               \___/    \___
  *
  *          G    /\       /\
  *              /  \     /  \
  *             /    \___/    \___
  *
  *          B       /\       /\
  *                 /  \     /  \
  *             ___/    \___/    \___
  *
  *         With this pattern only 2 colors are present at a time.
  *         One rising the other falling.
  *         Full rainbow spectrum is scanned.
  *         Luminosity remains constant.
  *******************************************************************
  * @param  None
  * @retval None
  */
void Demo_LedColor(void)
{
  int32_t  RisingColor = -1;
  uint32_t DemoPause = 0;

  /* Initialize LedColor demo */
  RGBLed_Init();

  /* Initialize */
  PhaseRGB[RED]   = PHASE_FALLING;
  PhaseRGB[GREEN] = PHASE_RISING;
  PhaseRGB[BLUE]  = PHASE_OFF;
  PerMilleRGB[RED]   = 1000;
  PerMilleRGB[GREEN] = 0;
  PerMilleRGB[BLUE]  = 0;
  RedWeight   =  90;
  GreenWeight = 100;
  BlueWeight  =  80;

  /* Set 50% brightness = 500 out of 1000 */
  GlobalBrigthness_1000 = 500;

  /***********************************************************/
  /* Start HRTIM waveforms generation                        */
  /***********************************************************/
  /* Set Burst IdleDuration: Global brightness control by HRTIM burst idle duration */
  HrtimHandle.Instance->sCommonRegs.BMCMPR = BURST_PERIOD - ((GammaCorrection1000[GlobalBrigthness_1000] * BURST_PERIOD) /1000);

  /* Only Red (falling) and Green (rising) Led are active at initial phase */
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, hrtim_output[RED]) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, hrtim_output[GREEN]) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, hrtim_output[BLUE]) != HAL_OK)
  {
    Error_Handler();
  }

  /**********************************************************/
  /* Automatic rainbow scanning                             */
  /**********************************************************/
  while(UserEvent != JOY_SEL )
  {
    /***********************************************************/
    /* Generate Sawtooth waveform for all RGB leds             */
    /***********************************************************/
    MaxVoltageSenseRGB[RED]   = (MAX_DAC_SAW_PEAK * PerMilleRGB[RED] * RedWeight) / (1000 * 100);
    MaxVoltageSenseRGB[GREEN] = (MAX_DAC_SAW_PEAK * PerMilleRGB[GREEN] * GreenWeight) / (1000 * 100);
    MaxVoltageSenseRGB[BLUE]  = (MAX_DAC_SAW_PEAK * PerMilleRGB[BLUE] * BlueWeight) / (1000 * 100);

    /* generate sawtooth signal to limit current in led and thus set color intensity */
    HAL_DACEx_SawtoothWaveGenerate(&DacRedHandle,   DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[RED]   * 4096) / 3300,   FAST_DAC_STEP);
    HAL_DACEx_SawtoothWaveGenerate(&DacGreenHandle, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[GREEN] * 4096) / 3300 ,  FAST_DAC_STEP);
    HAL_DACEx_SawtoothWaveGenerate(&DacBlueHandle,  DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[BLUE]  * 4096) / 3300 ,  SLOW_DAC_STEP);

    /* Wait between 2 consecutive color changes */
    HAL_Delay(5); //  3 colors * 1000 Per-Mille * 5ms / COLOR_STEP = 15 seconds for the whole rainbow spectrum

    /************************************************************************************/
    /* Compute next global brightness in case of user up/down key press                 */
    /* Pause/resume rainbow scanning in case of user left/right key press               */
    /************************************************************************************/
    if (UserAction != 0x00u)
    {
      while(BSP_JOY_GetState(JOY1) != JOY_NONE)
      {
        switch(UserEvent)
        {
        case JOY_DOWN :
          {
            /* Decrease global brightness */
            GlobalBrigthness_1000 = GlobalBrigthness_1000 - GLOBAL_BRIGHTNESS_STEP;
            BSP_LED_On(LED_BLUE);

            /* Blink led in case of limit reached */
            if (GlobalBrigthness_1000 < 0)
            {
              GlobalBrigthness_1000 = 0;
              HAL_Delay(50);
              BSP_LED_Off(LED_BLUE);
              HAL_Delay(50);
            }
            UserAction = 0;
            break;
          }
        case JOY_UP :
          {
            /* Decrease global brightness */
            GlobalBrigthness_1000 = GlobalBrigthness_1000 + GLOBAL_BRIGHTNESS_STEP;
            BSP_LED_On(LED_RED);

            /* Blink led in case of limit reached */
            if (GlobalBrigthness_1000 > 1000)
            {
              GlobalBrigthness_1000 = 1000;
              HAL_Delay(50);
              BSP_LED_Off(LED_RED);
              HAL_Delay(50);
            }
            UserAction = 0;
            break;
          }
        case JOY_LEFT :
          /* Pause rainbows spectrum scanning */
          DemoPause = 1;
          UserAction = 0;
          break;
        case JOY_RIGHT :
          /* Resume rainbows spectrum scanning */
          DemoPause = 0;
          UserAction = 0;
          break;
         default:
            break;
        }

        /* Set Burst IdleDuration: Global brightness control by HRTIM burst idle duration */
        HrtimHandle.Instance->sCommonRegs.BMCMPR = BURST_PERIOD - ((GammaCorrection1000[GlobalBrigthness_1000] * BURST_PERIOD) / 1000);

        /* Wait between 2 global brightness changes */
        HAL_Delay(4);
      }
      /* Switch off leds */
      BSP_LED_Off(LED_BLUE);
      BSP_LED_Off(LED_RED);
    }


    /* Don't update color when rainbow color scanning is paused */
    if (DemoPause != 1)
    {
      /****************************************************************************************/
      /* Compute next color party per mille for automatic rainbow color spectrum parsing      */
      /****************************************************************************************/
      for (uint32_t color = 0 ; color < 3; color++)
      {
        switch (PhaseRGB[color])
        {
        case PHASE_FALLING:
          PerMilleRGB[color] = PerMilleRGB[color] - COLOR_STEP;
          break;
        case PHASE_RISING:
          PerMilleRGB[color] = PerMilleRGB[color] + COLOR_STEP;
          break;
        case PHASE_OFF:
        default:
          break;
        }

        /* end of Falling phase */
        if (PerMilleRGB[color] < 0)
        {
          PerMilleRGB[color] = 0;
          PhaseRGB[color] = PHASE_OFF;

          /* (color+2)%3 should start rising next at step. */
          /* Wait for end of current step to change corresponding color phase */
          RisingColor = (color + 2) % 3;

          if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, hrtim_output[color]) != HAL_OK)
          {
            Error_Handler();
          }
        }

        /* end of Rising phase */
        if (PerMilleRGB[color] > 1000)
        {
          PerMilleRGB[color] = 1000;
          PhaseRGB[color] = PHASE_FALLING;
        }
      }

      /* After per mille computation update for all 3 colors, update OFF color to rising phase if any  */
      if (RisingColor >= 0)
      {
        /* Change previously OFF color phase to Rising phase  */
        PhaseRGB[RisingColor] = PHASE_RISING;

        /* Restart output waveform */
        if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, hrtim_output[RisingColor]) != HAL_OK)
        {
          Error_Handler();
        }

        /* Reset rising color */
        RisingColor = -1;
      }
    }
  }

/* Deinitialize LedColor demo */
  RGBLed_DeInit();
}


/**
  * @brief  RGB led white demonstration
  *         Automatic dimming of RBG leds with white
  * @param  None
  * @retval None
  */
void Demo_LedWhite(void)
{
  uint32_t DemoPause = 0;
  LED_PHASE_TypeDef Phase = PHASE_RISING;

  /* Initialize LedWhite demo */
  RGBLed_Init();

  UserAction = 0;
  GlobalBrigthness_1000 = 0;

  /* White balance. Could be adjusted to get cool/warm white */
  RedWeight   =  65;
  GreenWeight =  80;
  BlueWeight  =  50;

  /* Reset all signal LEDs but keep red led on as we start with rising brightness*/
  BSP_LED_On(LED_RED);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_ORANGE);

  /***********************************************************/
  /* Start HRTIM waveforms generation                        */
  /***********************************************************/
  /* Set Burst IdleDuration: Global brightness control by HRTIM burst idle duration */
  HrtimHandle.Instance->sCommonRegs.BMCMPR = BURST_PERIOD - ((GlobalBrigthness_1000 * BURST_PERIOD) / 1000);

  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, hrtim_output[RED]) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, hrtim_output[GREEN]) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, hrtim_output[BLUE]) != HAL_OK)
  {
    Error_Handler();
  }


  /***********************************************************/
  /* RGB white led automatic dimming                         */
  /***********************************************************/
  while(UserEvent != JOY_SEL )
  {
    /* For small brightness (under threshold), adjust sawtooth level */
    /* After threshold, sawtooth level stays at its maximum */
    if (GlobalBrigthness_1000 < BRIGHTNESS_THRESHOLD)
    {
      /***********************************************************/
      /* Configure Sawtooth waveform generation for all RGB leds */
      /***********************************************************/
      MaxVoltageSenseRGB[RED]   = (MAX_DAC_SAW_PEAK * GlobalBrigthness_1000 * RedWeight)   / (100 * BRIGHTNESS_THRESHOLD);
      MaxVoltageSenseRGB[GREEN] = (MAX_DAC_SAW_PEAK * GlobalBrigthness_1000 * GreenWeight) / (100 * BRIGHTNESS_THRESHOLD);
      MaxVoltageSenseRGB[BLUE]  = (MAX_DAC_SAW_PEAK * GlobalBrigthness_1000 * BlueWeight)  / (100 * BRIGHTNESS_THRESHOLD);

      /* Max current sense should be in the middle of saw tooth slope range */
      HAL_DACEx_SawtoothWaveGenerate(&DacRedHandle,   DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[RED]   * 4096) / 3300 ,  FAST_DAC_STEP);
      HAL_DACEx_SawtoothWaveGenerate(&DacGreenHandle, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[GREEN] * 4096) / 3300 ,  FAST_DAC_STEP);
      HAL_DACEx_SawtoothWaveGenerate(&DacBlueHandle,  DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[BLUE]  * 4096) / 3300 ,  SLOW_DAC_STEP);
    }

    /* Set Burst IdleDuration: Global brightness control by HRTIM burst idle duration */
    HrtimHandle.Instance->sCommonRegs.BMCMPR = BURST_PERIOD - ((GammaCorrection1000[GlobalBrigthness_1000] * BURST_PERIOD) / 1000);

    /* Wait between 2 global brightness changes */
    HAL_Delay(5);

    /************************************************************************************/
    /* Compute next global brightness in case of user up/down key press                 */
    /* Pause/resume automatic dimming in case of user left/right key press              */
    /************************************************************************************/
    if (UserAction != 0x00u)
    {
      /* While key is pressed */
      while(BSP_JOY_GetState(JOY1) != JOY_NONE)
      {
        switch(UserEvent)
        {
        case JOY_DOWN :
          {
            Phase = PHASE_FALLING;
            /* Decrease global brightness */
            GlobalBrigthness_1000 = GlobalBrigthness_1000 - GLOBAL_BRIGHTNESS_STEP;
            BSP_LED_On(LED_BLUE);
            BSP_LED_Off(LED_RED);

            /* Blink led in case of limit reached */
            if (GlobalBrigthness_1000 < 0)
            {
              GlobalBrigthness_1000 = 0;
              HAL_Delay(50);
              BSP_LED_Off(LED_BLUE);
              HAL_Delay(50);
            }
            UserAction = 0;
            break;
          }
        case JOY_UP :
          {
            Phase = PHASE_RISING;
            /* Increase global brightness */
            GlobalBrigthness_1000 = GlobalBrigthness_1000 + GLOBAL_BRIGHTNESS_STEP;
            BSP_LED_On(LED_RED);
            BSP_LED_Off(LED_BLUE);

            /* Blink led in case of limit reached */
            if (GlobalBrigthness_1000 > 1000)
            {

              GlobalBrigthness_1000 = 1000;
              HAL_Delay(50);
              BSP_LED_Off(LED_RED);
              HAL_Delay(50);
            }
            UserAction = 0;
            break;
          }
        case JOY_LEFT :
          /* Pause automatic dimming */
          DemoPause = 1;
          UserAction = 0;
          break;
        case JOY_RIGHT :
          /* Resume automatic dimming */
          DemoPause = 0;
          UserAction = 0;
          break;
        }

        /* Set Burst IdleDuration: Global brightness control by HRTIM burst idle duration */
        HrtimHandle.Instance->sCommonRegs.BMCMPR = BURST_PERIOD - ((GammaCorrection1000[GlobalBrigthness_1000] * BURST_PERIOD) / 1000);

        /* For small brightness (under threshold), adjust sawtooth level */
        /* After threshold, sawtooth level stays at its maximum */
        if (GlobalBrigthness_1000 < BRIGHTNESS_THRESHOLD)
        {
          /* Configure Sawtooth waveform generation for all RGB leds */
          MaxVoltageSenseRGB[RED]   = (MAX_DAC_SAW_PEAK * GlobalBrigthness_1000 * RedWeight)   / (100 * BRIGHTNESS_THRESHOLD);
          MaxVoltageSenseRGB[GREEN] = (MAX_DAC_SAW_PEAK * GlobalBrigthness_1000 * GreenWeight) / (100 * BRIGHTNESS_THRESHOLD);
          MaxVoltageSenseRGB[BLUE]  = (MAX_DAC_SAW_PEAK * GlobalBrigthness_1000 * BlueWeight)  / (100 * BRIGHTNESS_THRESHOLD);

          /* Max current sense should be in the middle of saw tooth slope range */
          HAL_DACEx_SawtoothWaveGenerate(&DacRedHandle,   DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[RED]   * 4096) / 3300 ,  FAST_DAC_STEP);
          HAL_DACEx_SawtoothWaveGenerate(&DacGreenHandle, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[GREEN] * 4096) / 3300 ,  FAST_DAC_STEP);
          HAL_DACEx_SawtoothWaveGenerate(&DacBlueHandle,  DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, (MaxVoltageSenseRGB[BLUE]  * 4096) / 3300 ,  SLOW_DAC_STEP);
        }

        /* Wait between 2 global brightness changes */
        HAL_Delay(5);
      }
    }


    /* Don't update brightness when auto dimming is paused */
    if (DemoPause != 1)
    {
      /************************************************************************************/
      /* Automatic increment or decrement brightness depending on rising or falling phase */
      /************************************************************************************/
      if (Phase == PHASE_RISING)
      {
        GlobalBrigthness_1000 = GlobalBrigthness_1000 + GLOBAL_BRIGHTNESS_STEP;
      }
      else
      {
        GlobalBrigthness_1000 = GlobalBrigthness_1000 - GLOBAL_BRIGHTNESS_STEP;
      }

      if (GlobalBrigthness_1000 > 1000)
      {
        GlobalBrigthness_1000 = 1000;
        Phase = PHASE_FALLING;
      }

      if (GlobalBrigthness_1000 < 0)
      {
        GlobalBrigthness_1000 = 0;
        Phase = PHASE_RISING;
      }

      /* Update led status */
      if (Phase == PHASE_RISING)
      {
        /* Red led indicates rising phase */
        BSP_LED_Off(LED_BLUE);
        BSP_LED_On(LED_RED);
      }
      else
      {
        /* Red led indicates falling phase */
        BSP_LED_On(LED_BLUE);
        BSP_LED_Off(LED_RED);
      }
    }
  }

  /* Deinitialize LedWhite demo */
  RGBLed_DeInit();

  /* Reset all signal LEDs */
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_ORANGE);
}


/**
  * @brief  RGB led initialization: DAC, COMP, HRTIM
  * @param  None
  * @retval None
  */
void RGBLed_Init(void)
{
  /* DAC Configuration */
  __HAL_RCC_DAC1_CLK_ENABLE();
  __HAL_RCC_DAC3_CLK_ENABLE();
  DAC_Red_Config();
  DAC_Green_Config();
  DAC_Blue_Config();

#ifdef DEBUG_FAST_DAC
  /* OPAMP Configuration for DAC3 channel1/2 debug as there is no GPIO output for DAC3 */
  OPAMP_Red_Config();
  OPAMP_Green_Config();
#endif

#ifdef DEBUG_SLOW_DAC
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Debug DAC1_CH1 on PA4 */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin  =  GPIO_PIN_4;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

  /* COMP Configuration */
  COMP_Red_Config();
  COMP_Green_Config();
  COMP_Blue_Config();

  /* HRTIM Configuration */
  HRTIM_Config();
}

/**
  * @brief  led deinitialization: DAC, COMP, HRTIM
  * @param  None
  * @retval None
  */
void RGBLed_DeInit(void)
{
  /* HRTIM Deconfiguration */
  HRTIM_DeConfig();

#ifdef DEBUG_FAST_DAC
  /* OPAMP Deconfiguration */
  OPAMP_Red_DeConfig();
  OPAMP_Green_DeConfig();

  /* Deinit GPIO PA4 */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
#endif

  /* DAC Deconfiguration */
  DAC_Red_DeConfig();
  DAC_Green_DeConfig();
  DAC_Blue_DeConfig();
  __HAL_RCC_DAC1_CLK_DISABLE();
  __HAL_RCC_DAC3_CLK_DISABLE();

  /* COMP Deconfiguration */
  COMP_Red_DeConfig();
  COMP_Green_DeConfig();
  COMP_Blue_DeConfig();
}

/**
* @brief  DAC configuration for Red color
* @param  None
* @retval None
*/
static void DAC_Red_Config(void)
{
  /* DAC3 channel2 is used for red led */
  DAC_ChannelConfTypeDef DAC_ConfigStructure = {0};

  /* DAC3 deinitialize */
  /* DAC3 is a fast DAC. Up to 15M samples per seconds */
  DacRedHandle.Instance = DAC3;

  /* DAC Init */
  if(HAL_DAC_Init(&DacRedHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Fill DAC ConfigStructure */
  /* Sawtooth waveform generation will be triggered by HRTIM F */
  DAC_ConfigStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  DAC_ConfigStructure.DAC_Trigger      = DAC_TRIGGER_HRTIM_RST_TRG6;  // TRG6 correspond to HRTIM F
  DAC_ConfigStructure.DAC_Trigger2     = DAC_TRIGGER_HRTIM_STEP_TRG6;
  DAC_ConfigStructure.DAC_ConnectOnChipPeripheral      = DAC_CHIPCONNECT_INTERNAL;
  DAC_ConfigStructure.DAC_SampleAndHold      = DAC_SAMPLEANDHOLD_DISABLE;
  if(HAL_DAC_ConfigChannel(&DacRedHandle, &DAC_ConfigStructure, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start Sawtooth waveform generation */
  HAL_DACEx_SawtoothWaveGenerate(&DacRedHandle, DAC_CHANNEL_2, DAC_SAWTOOTH_POLARITY_DECREMENT, 0, FAST_DAC_STEP);
  if(HAL_DAC_Start(&DacRedHandle, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  DAC configuration for Green color
* @param  None
* @retval None
*/
static void DAC_Green_Config(void)
{
  /* DAC3 channel1 is used for green led */
  DAC_ChannelConfTypeDef DAC_ConfigStructure = {0};

  /* DAC3 deinitialize */
  /* DAC3 is a fast DAC. Up to 15M samples per seconds */
  DacGreenHandle.Instance = DAC3;

  /* DAC Init */
  if(HAL_DAC_Init(&DacGreenHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Fill DAC ConfigStructure */
  /* Sawtooth waveform generation will be triggered by HRTIM E */
  DAC_ConfigStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  DAC_ConfigStructure.DAC_Trigger      = DAC_TRIGGER_HRTIM_RST_TRG5;  // TRG5 correspond to HRTIM E
  DAC_ConfigStructure.DAC_Trigger2     = DAC_TRIGGER_HRTIM_STEP_TRG5;
  DAC_ConfigStructure.DAC_ConnectOnChipPeripheral      = DAC_CHIPCONNECT_INTERNAL;
  DAC_ConfigStructure.DAC_SampleAndHold      = DAC_SAMPLEANDHOLD_DISABLE;
  if(HAL_DAC_ConfigChannel(&DacGreenHandle, &DAC_ConfigStructure, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start Sawtooth waveform generation */
  HAL_DACEx_SawtoothWaveGenerate(&DacGreenHandle, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, 0, FAST_DAC_STEP);
  if(HAL_DAC_Start(&DacGreenHandle, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  DAC configuration for Blue color
* @param  None
* @retval None
*/
static void DAC_Blue_Config(void)
{
  /* DAC1 channel1 is used for blue led */
  DAC_ChannelConfTypeDef DAC_ConfigStructure = {0};

  /* DAC1 is a slow DAC. Up to 1M samples per seconds. */
  /* Nevertheless it is possible to have better performances under conditions to see AN4566 */
  DacBlueHandle.Instance = DAC1;

  /* DAC Init */
  if(HAL_DAC_Init(&DacBlueHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Fill DAC ConfigStructure */
  /* Sawtooth waveform generation will be triggered by HRTIM A */
  DAC_ConfigStructure.DAC_Trigger      = DAC_TRIGGER_HRTIM_RST_TRG1;  // TRG1 correspond to HRTIM A
  DAC_ConfigStructure.DAC_Trigger2     = DAC_TRIGGER_HRTIM_STEP_TRG1;
#ifdef DEBUG_SLOW_DAC
  DAC_ConfigStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  DAC_ConfigStructure.DAC_ConnectOnChipPeripheral      = DAC_CHIPCONNECT_BOTH;
#else
  DAC_ConfigStructure.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  DAC_ConfigStructure.DAC_ConnectOnChipPeripheral      = DAC_CHIPCONNECT_INTERNAL;
#endif
  DAC_ConfigStructure.DAC_SampleAndHold      = DAC_SAMPLEANDHOLD_DISABLE;
  if(HAL_DAC_ConfigChannel(&DacBlueHandle, &DAC_ConfigStructure, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start Sawtooth waveform generation */
  HAL_DACEx_SawtoothWaveGenerate(&DacBlueHandle, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_DECREMENT, 0, SLOW_DAC_STEP);
  if(HAL_DAC_Start(&DacBlueHandle, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
* @brief  DAC deconfiguration for Red color
* @param  None
* @retval None
*/
static void DAC_Red_DeConfig(void)
{
  /* Deconfigure DAC3 channel2 */
  HAL_DAC_Stop(&DacRedHandle, DAC_CHANNEL_2);
  HAL_DAC_DeInit(&DacRedHandle);
}

/**
* @brief  DAC deconfiguration for Green color
* @param  None
* @retval None
*/
static void DAC_Green_DeConfig(void)
{
  /* Deconfigure DAC3 channel1 */
  HAL_DAC_Stop(&DacGreenHandle, DAC_CHANNEL_1);
  HAL_DAC_DeInit(&DacGreenHandle);
}

/**
* @brief  DAC deconfiguration for Blue color
* @param  None
* @retval None
*/
static void DAC_Blue_DeConfig(void)
{
  /* Deconfigure DAC1 channel1 */
  HAL_DAC_Stop(&DacBlueHandle, DAC_CHANNEL_1);
  HAL_DAC_DeInit(&DacBlueHandle);
}


/**
* @brief  COMP configuration for Red color
* @param  None
* @retval None
*/
static void COMP_Red_Config(void)
{
  /* COMP2 is used for red led sense */

  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure PA7 in analog mode: PA7 is connected to COMP2 non inverting input */
  GPIO_InitStructure.Pin  =  GPIO_PIN_7;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*  COMP2 deinitialize */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  CompRedHandle.Instance = COMP2;

  /* COMP2 config */
  CompRedHandle.Init.InputMinus        = COMP_INPUT_MINUS_DAC3_CH2;
  CompRedHandle.Init.InputPlus         = COMP_INPUT_PLUS_IO1;
  CompRedHandle.Init.OutputPol         = COMP_OUTPUTPOL_NONINVERTED;
  CompRedHandle.Init.Hysteresis        = COMP_HYSTERESIS_NONE;
  CompRedHandle.Init.BlankingSrce      = COMP_BLANKINGSRC_NONE;
  CompRedHandle.Init.TriggerMode       = COMP_TRIGGERMODE_NONE;
  if(HAL_COMP_Init(&CompRedHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable COMP2 */
  if(HAL_COMP_Start(&CompRedHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  COMP configuration for Green color
* @param  None
* @retval None
*/
static void COMP_Green_Config(void)
{
  /* COMP3 is used for green led sense */

  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Configure PA0 in analog mode: PA0 is connected to COMP3 non inverting input */
  GPIO_InitStructure.Pin  =  GPIO_PIN_0;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*  COMP3 deinitialize */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  CompGreenHandle.Instance = COMP3;

  /* COMP3 config */
  CompGreenHandle.Init.InputMinus        = COMP_INPUT_MINUS_DAC3_CH1;
  CompGreenHandle.Init.InputPlus         = COMP_INPUT_PLUS_IO1;
  CompGreenHandle.Init.OutputPol         = COMP_OUTPUTPOL_NONINVERTED;
  CompGreenHandle.Init.Hysteresis        = COMP_HYSTERESIS_NONE;
  CompGreenHandle.Init.BlankingSrce      = COMP_BLANKINGSRC_NONE;
  CompGreenHandle.Init.TriggerMode       = COMP_TRIGGERMODE_NONE;
  if(HAL_COMP_Init(&CompGreenHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable COMP3 */
  if(HAL_COMP_Start(&CompGreenHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  COMP configuration for Blue color
* @param  None
* @retval None
*/
static void COMP_Blue_Config(void)
{
  /* COMP4 is used for blue led sense */

  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOB Peripheral clock enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure PB0 in analog mode: PB0 is connected to COMP4 non inverting input */
  GPIO_InitStructure.Pin  =  GPIO_PIN_0;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*  COMP4 deinitialize */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  CompBlueHandle.Instance = COMP4;

  /* COMP4 config */
  CompBlueHandle.Init.InputMinus        = COMP_INPUT_MINUS_DAC1_CH1;
  CompBlueHandle.Init.InputPlus         = COMP_INPUT_PLUS_IO1;
  CompBlueHandle.Init.OutputPol         = COMP_OUTPUTPOL_NONINVERTED;
  CompBlueHandle.Init.Hysteresis        = COMP_HYSTERESIS_NONE;
  CompBlueHandle.Init.BlankingSrce      = COMP_BLANKINGSRC_NONE;
  CompBlueHandle.Init.TriggerMode       = COMP_TRIGGERMODE_NONE;
  if(HAL_COMP_Init(&CompBlueHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable COMP4 */
  if(HAL_COMP_Start(&CompBlueHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  COMP deconfiguration for Red color
* @param  None
* @retval None
*/
static void COMP_Red_DeConfig(void)
{
  /* Deconfigure COMP */
  HAL_COMP_Stop(&CompRedHandle);
  HAL_COMP_DeInit(&CompRedHandle);
}

/**
* @brief  COMP deconfiguration for Green color
* @param  None
* @retval None
*/
static void COMP_Green_DeConfig(void)
{
  /* Deconfigure COMP */
  HAL_COMP_Stop(&CompGreenHandle);
  HAL_COMP_DeInit(&CompGreenHandle);
}

/**
* @brief  COMP deconfiguration for Blue color
* @param  None
* @retval None
*/
static void COMP_Blue_DeConfig(void)
{
  /* Deconfigure COMP */
  HAL_COMP_Stop(&CompBlueHandle);
  HAL_COMP_DeInit(&CompBlueHandle);
}

#ifdef DEBUG_FAST_DAC
/**
* @brief  OPAMP configuration: follower mode.
*         Used for debug purpose to be able to view DAC output on GPIO
*         Warning: PB1 used for debug of red DAC (through OPAMP) is already used for Orange led
* @param  None
* @retval None
*/
static void OPAMP_Red_Config(void)
{
  GPIO_InitTypeDef       GPIO_InitStructure;

  /* OPAMP output configuration :  PB1 */
  /* GPIOB Peripheral clock enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStructure.Pin       = GPIO_PIN_1;
  GPIO_InitStructure.Mode      = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = 0x0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Enable OPAMP clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* Configure OPAMP in follower mode to view DAC output on GPIO */
  OpampRedHandle.Instance = OPAMP3;
  OpampRedHandle.Init.PowerMode = OPAMP_POWERMODE_HIGHSPEED;
  OpampRedHandle.Init.Mode = OPAMP_FOLLOWER_MODE;
  OpampRedHandle.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC ;
  OpampRedHandle.Init.InvertingInput = 0x0;
  OpampRedHandle.Init.InternalOutput = ENABLE;
  OpampRedHandle.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  OpampRedHandle.Init.InvertingInputSecondary = 0;
  OpampRedHandle.Init.NonInvertingInputSecondary = 0;
  OpampRedHandle.Init.PgaConnect = 0;
  OpampRedHandle.Init.PgaGain = 0;
  OpampRedHandle.Init.UserTrimming = OPAMP_TRIMMING_USER;
  OpampRedHandle.Init.TrimmingValueP = 0;
  OpampRedHandle.Init.TrimmingValueN = 0;
  HAL_OPAMP_Init(&OpampRedHandle);

  /* Calibrate OPAMP */
  HAL_OPAMP_SelfCalibrate(&OpampRedHandle);

/* To avoid blink of orange led during opamp calibration, the output is enabled only after calbration */
  OpampRedHandle.Init.InternalOutput = DISABLE;
  HAL_OPAMP_Init(&OpampRedHandle);

  /* Start OPAMP */
  HAL_OPAMP_Start(&OpampRedHandle);
}

/**
* @brief  OPAMP configuration: follower mode.
*         Used for debug purpose to be able to view DAC output on GPIO
* @param  None
* @retval None
*/
static void OPAMP_Green_Config(void)
{
  GPIO_InitTypeDef       GPIO_InitStructure;

  /* Enable OPAMP clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* OPAMP output configuration :  PA2 */
  /* GPIOA Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin       = GPIO_PIN_2;
  GPIO_InitStructure.Mode      = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = 0x0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure OPAMP in follower mode to view DAC output on GPIO */
  OpampGreenHandle.Instance = OPAMP1;
  OpampGreenHandle.Init.PowerMode = OPAMP_POWERMODE_HIGHSPEED;
  OpampGreenHandle.Init.Mode = OPAMP_FOLLOWER_MODE;
  OpampGreenHandle.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC ;
  OpampGreenHandle.Init.InvertingInput = 0x0;
  OpampGreenHandle.Init.InternalOutput = DISABLE;
  OpampGreenHandle.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  OpampGreenHandle.Init.InvertingInputSecondary = 0;
  OpampGreenHandle.Init.NonInvertingInputSecondary = 0;
  OpampGreenHandle.Init.PgaConnect = 0;
  OpampGreenHandle.Init.PgaGain = 0;
  OpampGreenHandle.Init.UserTrimming = OPAMP_TRIMMING_USER;
  OpampGreenHandle.Init.TrimmingValueP = 0;
  OpampGreenHandle.Init.TrimmingValueN = 0;
  HAL_OPAMP_Init(&OpampGreenHandle);

  /* Calibrate OPAMP */
  HAL_OPAMP_SelfCalibrate(&OpampGreenHandle);

  /* Start OPAMP */
  HAL_OPAMP_Start(&OpampGreenHandle);
}

/**
* @brief  OPAMP deconfiguration.
* @param  None
* @retval None
*/
static void OPAMP_Red_DeConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  HAL_OPAMP_Stop(&OpampRedHandle);
  HAL_OPAMP_DeInit(&OpampRedHandle);

  /* Deinit GPIO PB1 */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

  /* Restore LED orange configuration */
  GPIO_InitStructure.Pin       = GPIO_PIN_1;
  GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = 0x0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
* @brief  OPAMP deconfiguration.
* @param  None
* @retval None
*/
static void OPAMP_Green_DeConfig(void)
{
  HAL_OPAMP_Stop(&OpampGreenHandle);
  HAL_OPAMP_DeInit(&OpampGreenHandle);

  /* Deinit GPIO PA2 */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
}
#endif


/**
* @brief  HRTIM configuration
* @param  None
* @retval None
*/
static void HRTIM_Config(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  HRTIM_BurstModeCfgTypeDef HRTIM_BurstStructure;
  HRTIM_CompareCfgTypeDef   HRTIM_CompareStructure;
  HRTIM_EventCfgTypeDef     HRTIM_ExternalEventStructure;
  HRTIM_OutputCfgTypeDef    HRTIM_OutputStructure;
  HRTIM_TimeBaseCfgTypeDef  HRTIM_TimeBaseStructure;
  HRTIM_TimerCfgTypeDef     HRTIM_TimerWaveStructure;
  HRTIM_TimerCtlTypeDef     HRTIM_TimerCtl;
  HRTIM_TimerEventFilteringCfgTypeDef HRTIM_TimerEventFilteringCfg;

  /* Enable HRTIM clock */
  __HAL_RCC_HRTIM1_CLK_ENABLE();

  HrtimHandle.Instance = HRTIM1;
  HAL_HRTIM_Init(&HrtimHandle);

  /* HRTIM initialization startup */
  if(HAL_HRTIM_DLLCalibrationStart(&HrtimHandle, HRTIM_CALIBRATIONRATE_3) != HAL_OK)
  {
    Error_Handler();
  }

  if(HAL_HRTIM_PollForDLLCalibration(&HrtimHandle, HAL_MAX_DELAY) != HAL_OK)
  {
    Error_Handler();
  }

  /*******************************************************************************/
  /* Configure HRTIM TIM F                                                       */
  /* HRTIM output channel configuration : HRTIM_CHF1 (Buck drive red led) / PC6  */
  /*******************************************************************************/
  /* GPIOC Peripheral clock enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitStructure.Pin       = GPIO_PIN_6;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Pull      = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure the output features */
  HRTIM_OutputStructure.Polarity              = HRTIM_OUTPUTPOLARITY_HIGH;
  HRTIM_OutputStructure.SetSource             = HRTIM_OUTPUTSET_TIMPER | HRTIM_OUTPUTSET_TIMCMP1;
  HRTIM_OutputStructure.ResetSource           = HRTIM_OUTPUTRESET_EEV_1;  // EEV1 -> COMP2
  HRTIM_OutputStructure.IdleMode              = HRTIM_OUTPUTIDLEMODE_IDLE;
  HRTIM_OutputStructure.IdleLevel             = HRTIM_OUTPUTIDLELEVEL_INACTIVE;
  HRTIM_OutputStructure.FaultLevel            = HRTIM_OUTPUTFAULTLEVEL_NONE;
  HRTIM_OutputStructure.ChopperModeEnable     = HRTIM_OUTPUTCHOPPERMODE_DISABLED;
  HRTIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;

  if(HAL_HRTIM_WaveformOutputConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F,
                                    HRTIM_OUTPUT_TF1, &HRTIM_OutputStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure HRTIM1_TIMF waveform */
  HRTIM_TimerWaveStructure.InterruptRequests     = (HRTIM_MASTER_IT_NONE | HRTIM_TIM_IT_NONE);
  HRTIM_TimerWaveStructure.DMARequests           = HRTIM_MASTER_DMA_NONE;
  HRTIM_TimerWaveStructure.DMASrcAddress         = 0;
  HRTIM_TimerWaveStructure.DMADstAddress         = 0;
  HRTIM_TimerWaveStructure.DMASize               = 0;
  HRTIM_TimerWaveStructure.HalfModeEnable        = HRTIM_HALFMODE_DISABLED;
  HRTIM_TimerWaveStructure.InterleavedMode       = HRTIM_INTERLEAVED_MODE_DISABLED;
  HRTIM_TimerWaveStructure.StartOnSync           = HRTIM_SYNCSTART_DISABLED;
  HRTIM_TimerWaveStructure.ResetOnSync           = HRTIM_SYNCRESET_DISABLED;
  HRTIM_TimerWaveStructure.DACSynchro            = HRTIM_DACSYNC_NONE;
  HRTIM_TimerWaveStructure.PreloadEnable         = HRTIM_PRELOAD_DISABLED;
  HRTIM_TimerWaveStructure.UpdateGating          = HRTIM_UPDATEGATING_INDEPENDENT;
  HRTIM_TimerWaveStructure.BurstMode             = HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  HRTIM_TimerWaveStructure.RepetitionUpdate      = HRTIM_UPDATEONREPETITION_DISABLED;
  HRTIM_TimerWaveStructure.PushPull              = HRTIM_TIMPUSHPULLMODE_DISABLED;
  HRTIM_TimerWaveStructure.FaultEnable           = HRTIM_TIMFAULTENABLE_NONE;
  HRTIM_TimerWaveStructure.FaultLock             = HRTIM_TIMFAULTLOCK_READWRITE;
  HRTIM_TimerWaveStructure.DeadTimeInsertion     = HRTIM_TIMDEADTIMEINSERTION_DISABLED;
  HRTIM_TimerWaveStructure.DelayedProtectionMode = HRTIM_TIMER_F_DELAYEDPROTECTION_DISABLED;
  HRTIM_TimerWaveStructure.BalancedIdleAutomaticResume = HRTIM_OUTPUTBIAR_DISABLED;
  HRTIM_TimerWaveStructure.UpdateTrigger         = HRTIM_TIMUPDATETRIGGER_TIMER_F;
  HRTIM_TimerWaveStructure.ResetTrigger          = HRTIM_TIMRESETTRIGGER_NONE;
  HRTIM_TimerWaveStructure.ResetUpdate           = HRTIM_TIMUPDATEONRESET_DISABLED;
  HRTIM_TimerWaveStructure.ReSyncUpdate          = HRTIM_TIMERESYNC_UPDATE_UNCONDITIONAL;

  if(HAL_HRTIM_WaveformTimerConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, &HRTIM_TimerWaveStructure) != HAL_OK)
  {
    Error_Handler();
  }

  HRTIM_CompareStructure.AutoDelayedMode    = HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue       = (PERIOD/(FAST_DAC_STEP_NB));
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F,
                                     HRTIM_COMPAREUNIT_2, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  HRTIM_TimeBaseStructure.Period            = PERIOD; /* 1 period = 4 µs = 100% time */
  HRTIM_TimeBaseStructure.RepetitionCounter = 0x00;
  HRTIM_TimeBaseStructure.PrescalerRatio    = HRTIM_PRESCALERRATIO_MUL32;
  HRTIM_TimeBaseStructure.Mode              = HRTIM_MODE_CONTINUOUS;

  if(HAL_HRTIM_TimeBaseConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, &HRTIM_TimeBaseStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure External Event Source 1 */
  HRTIM_ExternalEventStructure.Source      = HRTIM_EEV1SRC_COMP2_OUT;
  HRTIM_ExternalEventStructure.Polarity    = HRTIM_EVENTPOLARITY_HIGH;
  HRTIM_ExternalEventStructure.Sensitivity = HRTIM_EVENTSENSITIVITY_LEVEL;
  HRTIM_ExternalEventStructure.FastMode    = HRTIM_EVENTFASTMODE_ENABLE;
  HRTIM_ExternalEventStructure.Filter      = HRTIM_EVENTFILTER_NONE;

  if(HAL_HRTIM_EventConfig(&HrtimHandle, HRTIM_EVENT_1, &HRTIM_ExternalEventStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 3 is used to blank COMP event at the beginning of the period */
  /* This is to avoid being reset by spurious glitch */

  HRTIM_TimerEventFilteringCfg.Filter = HRTIM_TIMEEVFLT_BLANKINGCMP3;
  HRTIM_TimerEventFilteringCfg.Latch = HRTIM_TIMEVENTLATCH_DISABLED;
  if(HAL_HRTIM_TimerEventFilteringConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, HRTIM_EVENT_1, &HRTIM_TimerEventFilteringCfg) != HAL_OK)
  {
    Error_Handler();
  }
  HRTIM_CompareStructure.CompareValue       = 0x500;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F,
                                     HRTIM_COMPAREUNIT_3, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 1 is used to force output of HRTIM few cycles after roll over. */
  /* This is to be sure timer output is set despite reset (COMP) event occurring at the very beginning of the period */
  HRTIM_CompareStructure.CompareValue       = 0x100;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F,
                                     HRTIM_COMPAREUNIT_1, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Burst Mode configuration */
  HRTIM_BurstStructure.Mode          = HRTIM_BURSTMODE_CONTINOUS;
  HRTIM_BurstStructure.ClockSource   = HRTIM_BURSTMODECLOCKSOURCE_TIMER_F;
  HRTIM_BurstStructure.Prescaler     = HRTIM_BURSTMODEPRESCALER_DIV1;
  HRTIM_BurstStructure.PreloadEnable = HRIM_BURSTMODEPRELOAD_ENABLED;
  HRTIM_BurstStructure.Trigger       = HRTIM_BURSTMODETRIGGER_NONE;
  HRTIM_BurstStructure.IdleDuration  = BURST_PERIOD;
  HRTIM_BurstStructure.Period        = BURST_PERIOD;

  if(HAL_HRTIM_BurstModeConfig(&HrtimHandle, &HRTIM_BurstStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the TF1 output */
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, HRTIM_OUTPUT_TF1) != HAL_OK)
  {
    Error_Handler();
  }

  if(HAL_HRTIM_BurstModeCtl(&HrtimHandle, HRTIM_BURSTMODECTL_ENABLED) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure DAC Reset and Step signal for sawtooth generation */
  HRTIM_TimerCtl.UpDownMode = HRTIM_TIMERUPDOWNMODE_UP;
  HRTIM_TimerCtl.TrigHalf = HRTIM_TIMERTRIGHALF_DISABLED;
  HRTIM_TimerCtl.GreaterCMP3 = HRTIM_TIMERGTCMP3_EQUAL;
  HRTIM_TimerCtl.GreaterCMP1 = HRTIM_TIMERGTCMP1_EQUAL;
  HRTIM_TimerCtl.DualChannelDacReset = HRTIM_TIMER_DCDR_COUNTER;
  HRTIM_TimerCtl.DualChannelDacStep = HRTIM_TIMER_DCDS_CMP2;
  HRTIM_TimerCtl.DualChannelDacEnable = HRTIM_TIMER_DCDE_ENABLED;

  if(HAL_HRTIM_TimerDualChannelDacConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_F, &HRTIM_TimerCtl) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Burst Trigger */
  if(HAL_HRTIM_BurstModeSoftwareTrigger(&HrtimHandle) != HAL_OK)
  {
    Error_Handler();
  }

  /*********************************************************************************/
  /* Configure HRTIM TIM E                                                         */
  /* HRTIM output channel configuration : HRTIM_CHE1 (Buck drive green led) / PC8  */
  /*********************************************************************************/
  GPIO_InitStructure.Pin       = GPIO_PIN_8;
  GPIO_InitStructure.Alternate = GPIO_AF3_HRTIM1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure the output features */
  HRTIM_OutputStructure.ResetSource           = HRTIM_OUTPUTRESET_EEV_5;  // EEV5 -> COMP3
  if(HAL_HRTIM_WaveformOutputConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                    HRTIM_OUTPUT_TE1, &HRTIM_OutputStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure HRTIM1_TIME waveform */
  HRTIM_TimerWaveStructure.DelayedProtectionMode = HRTIM_TIMER_D_E_DELAYEDPROTECTION_DISABLED;
  HRTIM_TimerWaveStructure.UpdateTrigger         = HRTIM_TIMUPDATETRIGGER_TIMER_E;
  if(HAL_HRTIM_WaveformTimerConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimerWaveStructure) != HAL_OK)
  {
    Error_Handler();
  }

  HRTIM_CompareStructure.CompareValue       = (PERIOD/(FAST_DAC_STEP_NB));
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                     HRTIM_COMPAREUNIT_2, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  if(HAL_HRTIM_TimeBaseConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimeBaseStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure External Event Source 5 */
  HRTIM_ExternalEventStructure.Source      = HRTIM_EEV5SRC_COMP3_OUT;
  if(HAL_HRTIM_EventConfig(&HrtimHandle, HRTIM_EVENT_5, &HRTIM_ExternalEventStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure blanking on COMP event to avoid spurious glitch */
  if(HAL_HRTIM_TimerEventFilteringConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, HRTIM_EVENT_5, &HRTIM_TimerEventFilteringCfg) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 3 is used to blank COMP event at the beginning of the period */
  /* This is to avoid being reset by spurious glitch */
  HRTIM_CompareStructure.CompareValue       = 0x500;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                     HRTIM_COMPAREUNIT_3, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 1 is used to force output of HRTIM few cycles after roll over. */
  /* This is to be sure timer output is set despite reset (COMP) event occurring at the very beginning of the period */
  HRTIM_CompareStructure.CompareValue       = 0x100;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E,
                                     HRTIM_COMPAREUNIT_1, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the TE1 output */
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, HRTIM_OUTPUT_TE1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure DAC Reset and Step signal for sawtooth generation */
  if(HAL_HRTIM_TimerDualChannelDacConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_E, &HRTIM_TimerCtl) != HAL_OK)
  {
    Error_Handler();
  }

  /********************************************************************************/
  /* Configure HRTIM TIM A                                                        */
  /* HRTIM output channel configuration : HRTIM_CHA1 (Buck drive blue led) / PA8  */
  /************************************************************************* ******/
  /* GPIOA Peripheral clock enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin       = GPIO_PIN_8;
  GPIO_InitStructure.Alternate = GPIO_AF13_HRTIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure the output features */
  HRTIM_OutputStructure.ResetSource           = HRTIM_OUTPUTRESET_EEV_2;  // EEV2 -> COMP4
  if(HAL_HRTIM_WaveformOutputConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A,
                                    HRTIM_OUTPUT_TA1, &HRTIM_OutputStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure HRTIM1_TIMA waveform */
  HRTIM_TimerWaveStructure.DelayedProtectionMode = HRTIM_TIMER_A_B_C_DELAYEDPROTECTION_DISABLED;
  HRTIM_TimerWaveStructure.UpdateTrigger         = HRTIM_TIMUPDATETRIGGER_TIMER_A;
  if(HAL_HRTIM_WaveformTimerConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_TimerWaveStructure) != HAL_OK)
  {
    Error_Handler();
  }

  HRTIM_CompareStructure.CompareValue       = (PERIOD/(SLOW_DAC_STEP_NB));
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A,
                                     HRTIM_COMPAREUNIT_2, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  if(HAL_HRTIM_TimeBaseConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_TimeBaseStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure External Event Source 2 */
  HRTIM_ExternalEventStructure.Source      = HRTIM_EEV2SRC_COMP4_OUT;
  if(HAL_HRTIM_EventConfig(&HrtimHandle, HRTIM_EVENT_2, &HRTIM_ExternalEventStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure blanking on COMP event to avoid spurious glitch */
  if(HAL_HRTIM_TimerEventFilteringConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A, HRTIM_EVENT_2, &HRTIM_TimerEventFilteringCfg) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 3 is used to blank COMP event at the beginning of the period */
  /* This is to avoid being reset by spurious glitch */
  HRTIM_CompareStructure.CompareValue       = 0x500;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A,
                                     HRTIM_COMPAREUNIT_3, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Compare unit 1 is used to force output of HRTIM few cycles after roll over. */
  /* This is to be sure timer output is set despite reset (COMP) event occurring at the very beginning of the period */
  HRTIM_CompareStructure.CompareValue       = 0x100;
  if(HAL_HRTIM_WaveformCompareConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A,
                                     HRTIM_COMPAREUNIT_1, &HRTIM_CompareStructure) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the TA1 output */
  if(HAL_HRTIM_WaveformOutputStart(&HrtimHandle, HRTIM_OUTPUT_TA1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure DAC Reset and Step signal for sawtooth generation */
  if(HAL_HRTIM_TimerDualChannelDacConfig(&HrtimHandle, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_TimerCtl) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start all slaves in one shot so that they are all synchronised */
  if(HAL_HRTIM_WaveformCountStart(&HrtimHandle, HRTIM_TIMERID_TIMER_A | HRTIM_TIMERID_TIMER_E | HRTIM_TIMERID_TIMER_F) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select Burst Trigger */
  if(HAL_HRTIM_BurstModeSoftwareTrigger(&HrtimHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  HRTIM deconfiguration
* @param  None
* @retval None
*/
static void HRTIM_DeConfig(void)
{
  /* BUCK LED is OFF */
  HrtimHandle.Instance->sCommonRegs.BMCMPR = BURST_PERIOD;
  HAL_Delay(10);

  /* Stop HRTIM A1 waveform generation */
  if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, HRTIM_OUTPUT_TA1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_HRTIM_WaveformCountStop(&HrtimHandle, HRTIM_TIMERID_TIMER_A);

  /* Stop HRTIM E1 waveform generation */
  if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, HRTIM_OUTPUT_TE1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_HRTIM_WaveformCountStop(&HrtimHandle, HRTIM_TIMERID_TIMER_E);

  /* Stop HRTIM F1 waveform generation */
  if(HAL_HRTIM_WaveformOutputStop(&HrtimHandle, HRTIM_OUTPUT_TF1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_HRTIM_WaveformCountStop(&HrtimHandle, HRTIM_TIMERID_TIMER_F);

  /* Stop burst mode and deinitialize HRTIM */
  HAL_HRTIM_BurstModeCtl(&HrtimHandle, HRTIM_BURSTMODECTL_DISABLED);
  HAL_HRTIM_DeInit(&HrtimHandle);
  __HAL_RCC_HRTIM1_CLK_DISABLE();
  __HAL_RCC_HRTIM1_FORCE_RESET();
  HAL_Delay(1);
  __HAL_RCC_HRTIM1_RELEASE_RESET();

  /* Deinit GPIO */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6);
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8);
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
}


