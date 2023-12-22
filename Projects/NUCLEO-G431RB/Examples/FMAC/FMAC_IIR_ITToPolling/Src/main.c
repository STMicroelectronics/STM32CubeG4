/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FMAC/FMAC_IIR_ITToPolling/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use FMAC HAL API (FMAC instance)
  *          to perform an IIR filter from IT mode to polling mode.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32g4xx_nucleo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
FMAC_HandleTypeDef hfmac;

/* USER CODE BEGIN PV */
/* FMAC configuration structure */
FMAC_FilterConfigTypeDef sFmacConfig;

/* Array of filter coefficients A (feedback coefficients) in Q1.15 format */
static int16_t aFilterCoeffA[COEFF_VECTOR_A_SIZE] =
{
   19448,-29793,  9645, -4884,   671,   -81
};

/* Array of filter coefficients B (feed-forward taps) in Q1.15 format */
static int16_t aFilterCoeffB[COEFF_VECTOR_B_SIZE] =
{
     590,  3540,  8851, 11801,  8851,  3540,   590
};

/* Array of input values in Q1.15 format (in three parts in order to write new data during the calculation) */
static int16_t aInputValues1[INPUT_ARRAY_1_SIZE] =
{
       0,  5276, -1548, 13844,     7, 17551,  5802, 16142, 14198, 12009,
   21624,  8678, 24576,  8672, 21611, 11990, 14172, 16111,  5765, 17510,
     -37, 13797, -1598,  5225,   -51, -5327,  1498,-13892,   -52,-17592,
   -5838,-16174,-14223,-12029,-21637, -8685,-24576, -8665,-21597,-11970,
  -14146,-16080, -5729,-17469,    82,-13749,  1647, -5174,   103,  5378,
   -1449, 13939,    96, 17632,  5874, 16205, 14249, 12048, 21650,  8691,
   24575,  8658, 21583, 11950, 14120, 16048,  5692, 17428,  -127, 13701,
   -1697,  5122,  -154, -5429,  1399,-13987,  -141,-17673, -5910,-16236,
  -14274,-12068,-21663, -8698,-24575, -8651,-21570,-11930,-14094,-16016,
   -5655,-17387,   171,-13654,  1747, -5071,   206,  5480, -1349, 14034,
     185, 17713,  5946, 16267, 14299, 12087, 21676,  8704, 24574,  8643,
   21556, 11909, 14067, 15984,  5618, 17346,  -216, 13606, -1797,  5020,
    -257
};
static int16_t aInputValues2[INPUT_ARRAY_2_SIZE] =
{
          -5530,  1300,-14081,  -229,-17754, -5982,-16297,-14324,-12106,
  -21688, -8710,-24574, -8636,-21542,-11889,-14041,-15952, -5581,-17304,
     261,-13558,  1847, -4969,   309,  5581, -1250, 14128,   273, 17794,
    6018, 16328, 14349, 12124, 21701,  8715, 24573,  8628, 21527, 11868,
   14014, 15920,  5544, 17263,  -306, 13510, -1897,  4918,  -360, -5632,
    1201,-14176,  -317,-17834, -6053,-16358,-14374,-12143,-21713, -8721,
  -24571, -8620,-21513,-11847,-13988,-15888, -5507,-17221,   352,-13462,
    1947, -4867,   412,  5683, -1152, 14223,   361, 17874,  6089, 16389,
   14399, 12162, 21725,  8726, 24570,  8612, 21498, 11826, 13961, 15856,
    5470, 17180,  -397, 13414, -1997,  4816,  -463, -5734,  1102,-14270,
    -405,-17914, -6124,-16419,-14423,-12180,-21737, -8732,-24569, -8604,
  -21484,-11805,-13934,-15823, -5432,-17138,   442,-13366,  2047, -4764,
     515,  5785, -1053, 14317,   449, 17954,  6160, 16449, 14447, 12198,
   21749,  8737, 24567,  8596, 21469, 11784, 13907, 15790,  5395, 17096,
    -488, 13318, -2097,  4713,  -566, -5836,  1004,-14363,  -493,-17994,
   -6195,-16479,-14472,-12216,-21760, -8742,-24565, -8587,-21454,-11763,
  -13879,-15758, -5357,-17054,   533,-13269,  2147, -4662,   618,  5886,
    -955, 14410,   536, 18033,  6230, 16509, 14496, 12234, 21772,  8747,
   24563,  8579, 21438, 11741, 13852, 15725,  5319, 17012,  -579, 13221,
   -2198,  4611,  -669, -5937,   905
};
static int16_t aInputValues3[INPUT_ARRAY_3_SIZE] =
{
                                     -14457,  -580,-18073, -6265,-16538,
  -14520,-12252,-21783, -8751,-24561, -8570,-21423,-11720,-13824,-15692,
   -5282,-16970,   624,-13173,  2248, -4559,   720,  5988,  -856, 14504,
     623, 18112,  6299, 16568, 14543, 12270, 21794,  8756, 24559,  8561,
   21408, 11698, 13797, 15659,  5244, 16928,  -670, 13124, -2298,  4508,
    -772, -6038,   807,-14550,  -667,-18152, -6334,-16597,-14567,-12287,
  -21805, -8760,-24557, -8552,-21392,-11676,-13769,-15625, -5205,-16886,
     716,-13076,  2348, -4457,   823,  6089,  -758, 14597,   710, 18191,
    6369, 16626, 14590, 12304, 21816,  8764, 24554,  8542, 21376, 11654,
   13741, 15592,  5167, 16843,  -761, 13027, -2399,  4405,  -875, -6140,
     709,-14643,  -753,-18230, -6403,-16656,-14614,-12321,-21827, -8768,
  -24551, -8533,-21360,-11632,-13713,-15559, -5129,-16801,   807,-12979,
    2449, -4354,   926,  6190,  -660, 14690,   796, 18269,  6437, 16685,
   14637, 12338, 21837,  8772, 24548,  8523, 21344, 11609, 13684, 15525,
    5090, 16758,  -853, 12930, -2500,  4303,  -977, -6241,   612,-14736,
    -839,-18308, -6472,-16713,-14660,-12355,-21847, -8776,-24545, -8514,
  -21328,-11587,-13656,-15491, -5052,-16715,   899,-12882,  2550, -4251,
    1029,  6291,  -563, 14782,   882, 18347,  6506, 16742, 14683, 12372,
   21858,  8779, 24542,  8504, 21311, 11564, 13628, 15457,  5013, 16673,
    -946, 12833, -2600,  4200, -1080, -6342,   514,-14828,  -925,-18386,
   -6540,-16771
};

/* Size of the currently handled input buffer aInputValuesX */
uint16_t CurrentInputArraySize;

/* Array of output data to preload in Q1.15 format */
static int16_t aOutputDataToPreload[COEFF_VECTOR_A_SIZE] =
{
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

/* Array of calculated filtered data in Q1.15 format (four parts) */
static int16_t aCalculatedFilteredData1[OUTPUT_ARRAY_1_SIZE];
static int16_t aCalculatedFilteredData2[OUTPUT_ARRAY_2_SIZE];
static int16_t aCalculatedFilteredData3[OUTPUT_ARRAY_3_SIZE];
static int16_t aCalculatedFilteredData4[OUTPUT_ARRAY_4_SIZE];

/* Expected number of calculated samples */
uint16_t ExpectedCalculatedOutputSize;

/* Status of the FMAC callbacks */
__IO uint32_t GetDataCallbackCount         = 0;
__IO uint32_t ErrorCount                   = 0;

/* Array of reference filtered data for IIR "7 feed-forward taps, 6 feedback coefficients, gain = 1" in Q1.15 format */
static const int16_t aRefFilteredData[ARRAY_SIZE] =
{
    7140, 13565, 12916, 10624, 12289, 15712, 16843, 15986, 15635, 16225,
   16376, 15489, 14241, 13174, 12045, 10498,  8621,  6684,  4744,  2697,
     532, -1645, -3752, -5782, -7735, -9568,-11228,-12685,-13927,-14937,
  -15692,-16174,-16376,-16301,-15947,-15319,-14427,-13288,-11923,-10352,
   -8603, -6707, -4698, -2607,  -470,  1674,  3789,  5839,  7789,  9605,
   11257, 12716, 13957, 14959, 15704, 16179, 16377, 16295, 15934, 15299,
   14401, 13257, 11886, 10311,  8559,  6659,  4646,  2554,   418, -1726,
   -3840, -5888, -7836, -9649,-11296,-12749,-13985,-14981,-15719,-16188,
  -16380,-16292,-15923,-15281,-14378,-13229,-11852,-10271, -8516, -6615,
   -4599, -2504,  -367,  1776,  3889,  5935,  7880,  9689, 11332, 12781,
   14011, 15001, 15733, 16195, 16380, 16285, 15910, 15262, 14352, 13196,
   11815, 10231,  8471,  6566,  4549,  2453,   315, -1828, -3940, -5984,
   -7925, -9730,-11370,-12815,-14039,-15021,-15747,-16205,-16383,-16280,
  -15898,-15244,-14329,-13167,-11779,-10190, -8428, -6520, -4500, -2403,
    -265,  1878,  3989,  6031,  7970,  9771, 11406, 12845, 14064, 15042,
   15761, 16210, 16382, 16274, 15885, 15223, 14301, 13135, 11744, 10150,
    8382,  6471,  4450,  2351,   212, -1930, -4039, -6079, -8016, -9814,
  -11443,-12877,-14091,-15063,-15776,-16219,-16384,-16268,-15872,-15205,
  -14278,-13106,-11709,-10110, -8339, -6425, -4401, -2301,  -161,  1981,
    4088,  6126,  8060,  9854, 11479, 12908, 14117, 15083, 15789, 16225,
   16383, 16261, 15859, 15185, 14251, 13074, 11673, 10070,  8294,  6376,
    4350,  2249,   109, -2032, -4139, -6175, -8106, -9896,-11517,-12941,
  -14143,-15103,-15804,-16234,-16385,-16255,-15847,-15168,-14228,-13043,
  -11635,-10029, -8251, -6330, -4301, -2199,   -59,  2083,  4189,  6222,
    8149,  9936, 11553, 12972, 14169, 15122, 15815, 16239, 16385, 16249,
   15833, 15146, 14200, 13011, 11599,  9987,  8205,  6282,  4251,  2147,
       6, -2135, -4239, -6270, -8194, -9978,-11591,-13004,-14195,-15143,
  -15830,-16246,-16384,-16242,-15821,-15128,-14176,-12981,-11563, -9947,
   -8161, -6235, -4202, -2097,    44,  2185,  4288,  6316,  8237, 10017,
   11626, 13035, 14220, 15161, 15843, 16253, 16384, 16234, 15806, 15107,
   14149, 12948, 11526,  9906,  8116,  6186,  4151,  2045,   -97, -2237,
   -4338, -6365, -8284,-10060,-11663,-13066,-14246,-15182,-15857,-16260,
  -16384,-16228,-15793,-15088,-14124,-12918,-11491, -9866, -8072, -6140,
   -4103, -1994,   148,  2286,  4386,  6412,  8328, 10099, 11697, 13096,
   14271, 15200, 15868, 16265, 16383, 16220, 15779, 15068, 14098, 12885,
   11452,  9824,  8027,  6091,  4051,  1942,  -200, -2338, -4437, -6460,
   -8372,-10140,-11735,-13129,-14297,-15220,-15882,-16272,-16383,-16214,
  -15766,-15048,-14072,-12854,-11416, -9783, -7983, -6044, -4002, -1892,
     250,  2388,  4486,  6507,  8416, 10180, 11770, 13158, 14321, 15238,
   15893, 16277, 16382, 16205, 15750, 15026, 14044, 12821, 11379,  9741,
    7936,  5995,  3952,  1840,  -303, -2441, -4537, -6554, -8460,-10221,
  -11807,-13190,-14347,-15258,-15907,-16284,-16382,-16199,-15737,-15006,
  -14019,-12791,-11343, -9700, -7892, -5949, -3903, -1790,   353,  2490,
    4585,  6601,  8504, 10260, 11841, 13219, 14371, 15276, 15918, 16288,
   16379, 16190, 15722, 14984, 13990, 12757, 11305,  9658,  7846,  5899,
    3852,  1738,  -406, -2542, -4635, -6649, -8549,-10301,-11877,-13250,
  -14396,-15295,-15931,-16295,-16379,-16183,-15709,-14965,-13965,-12726,
  -11268, -9617, -7802, -5853, -3803, -1687,   457,  2592,  4683,  6694,
    8591, 10341, 11913, 13280, 14420, 15313, 15943, 16300, 16377, 16173,
   15692, 14943, 13937, 12692, 11230,  9575,  7755,  5802,  3752,  1637,
    -508, -2645, -4735, -6742, -8635,-10381
};

/* Auxiliary counter */
uint32_t Index;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_FMAC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* STM32G4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LED2 */
  BSP_LED_Init(LED2);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_FMAC_Init();
  /* USER CODE BEGIN 2 */

  /*## Configure the FMAC peripheral #########################################*/
  sFmacConfig.InputBaseAddress  = INPUT_BUFFER_BASE;
  sFmacConfig.InputBufferSize   = INPUT_BUFFER_SIZE;
  sFmacConfig.InputThreshold    = INPUT_THRESHOLD;
  sFmacConfig.CoeffBaseAddress  = COEFFICIENT_BUFFER_BASE;
  sFmacConfig.CoeffBufferSize   = COEFFICIENT_BUFFER_SIZE;
  sFmacConfig.OutputBaseAddress = OUTPUT_BUFFER_BASE;
  sFmacConfig.OutputBufferSize  = OUTPUT_BUFFER_SIZE;
  sFmacConfig.OutputThreshold   = OUTPUT_THRESHOLD;
  sFmacConfig.pCoeffA           = aFilterCoeffA;
  sFmacConfig.CoeffASize        = COEFF_VECTOR_A_SIZE;
  sFmacConfig.pCoeffB           = aFilterCoeffB;
  sFmacConfig.CoeffBSize        = COEFF_VECTOR_B_SIZE;
  sFmacConfig.Filter            = FMAC_FUNC_IIR_DIRECT_FORM_1;
  sFmacConfig.InputAccess       = FMAC_BUFFER_ACCESS_IT;
  sFmacConfig.OutputAccess      = FMAC_BUFFER_ACCESS_POLLING;
  sFmacConfig.Clip              = FMAC_CLIP_DISABLED;
  sFmacConfig.P                 = COEFF_VECTOR_B_SIZE;
  sFmacConfig.Q                 = COEFF_VECTOR_A_SIZE;
  sFmacConfig.R                 = GAIN;

  if (HAL_FMAC_FilterConfig(&hfmac, &sFmacConfig) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /*## Preload the input and output buffers ##################################*/
  if (HAL_FMAC_FilterPreload(&hfmac, aInputValues1, INPUT_ARRAY_1_SIZE,
                             aOutputDataToPreload, COEFF_VECTOR_A_SIZE) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /*## Start calculation of IIR filter in IT/Polling mode ####################*/
  ExpectedCalculatedOutputSize = OUTPUT_ARRAY_1_SIZE;
  if (HAL_FMAC_FilterStart(&hfmac, aCalculatedFilteredData1, &ExpectedCalculatedOutputSize) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Poll to read the 1st part of the output data ##########################*/
  if (HAL_FMAC_PollFilterData(&hfmac, POLLING_TIMEOUT) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Provide the 2nd output buffer to fill #################################*/
  ExpectedCalculatedOutputSize = OUTPUT_ARRAY_2_SIZE;
  if (HAL_FMAC_ConfigFilterOutputBuffer(&hfmac,
                                        aCalculatedFilteredData2,
                                        &ExpectedCalculatedOutputSize) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Poll to read the 2nd part of the output data ##########################*/
  if (HAL_FMAC_PollFilterData(&hfmac, POLLING_TIMEOUT) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Provide the 3rd output buffer to fill #################################*/
  ExpectedCalculatedOutputSize = OUTPUT_ARRAY_3_SIZE;
  if (HAL_FMAC_ConfigFilterOutputBuffer(&hfmac,
                                        aCalculatedFilteredData3,
                                        &ExpectedCalculatedOutputSize) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Poll to read the 3rd part of the output data ##########################*/
  if (HAL_FMAC_PollFilterData(&hfmac, POLLING_TIMEOUT) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Provide the 4th output buffer to fill #################################*/
  ExpectedCalculatedOutputSize = OUTPUT_ARRAY_4_SIZE;
  if (HAL_FMAC_ConfigFilterOutputBuffer(&hfmac,
                                        aCalculatedFilteredData4,
                                        &ExpectedCalculatedOutputSize) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Poll to read the 3rd part of the output data ##########################*/
  if (HAL_FMAC_PollFilterData(&hfmac, POLLING_TIMEOUT) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Wait for the end of the handling (no new data written) #################*/
  /*  For simplicity reasons, this example is just waiting till the end of the
      calculation, but the application may perform other tasks while the operation
      is ongoing. */
  while(GetDataCallbackCount == GET_DATA_CALLBACK_COUNT)
  {
  }

  /*## Stop the calculation of IIR filter in polling/DMA mode ##################*/
  if (HAL_FMAC_FilterStop(&hfmac) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Check the final error status ############################################*/
  if(ErrorCount != 0)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*## Compare FMAC results to the reference values #####################*/
  for (Index = 0; Index < OUTPUT_ARRAY_1_SIZE; Index++)
  {
    if (aCalculatedFilteredData1[Index]  != aRefFilteredData[Index])
    {
      /* Processing Error */
      Error_Handler();
    }
  }
  for (Index = 0; Index < OUTPUT_ARRAY_2_SIZE; Index++)
  {
    if (aCalculatedFilteredData2[Index]  != aRefFilteredData[OUTPUT_ARRAY_1_SIZE + Index])
    {
      /* Processing Error */
      Error_Handler();
    }
  }
  for (Index = 0; Index < OUTPUT_ARRAY_3_SIZE; Index++)
  {
    if (aCalculatedFilteredData3[Index]  != aRefFilteredData[OUTPUT_ARRAY_1_SIZE + OUTPUT_ARRAY_2_SIZE + Index])
    {
      /* Processing Error */
      Error_Handler();
    }
  }
  for (Index = 0; Index < OUTPUT_ARRAY_4_SIZE; Index++)
  {
    if (aCalculatedFilteredData4[Index]  != aRefFilteredData[OUTPUT_ARRAY_1_SIZE + OUTPUT_ARRAY_2_SIZE + OUTPUT_ARRAY_3_SIZE + Index])
    {
      /* Processing Error */
      Error_Handler();
    }
  }

  /* There is no error in the output values: Turn LED2 on */
  BSP_LED_On(LED2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FMAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_FMAC_Init(void)
{

  /* USER CODE BEGIN FMAC_Init 0 */

  /* USER CODE END FMAC_Init 0 */

  /* USER CODE BEGIN FMAC_Init 1 */

  /* USER CODE END FMAC_Init 1 */
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FMAC_Init 2 */

  /* USER CODE END FMAC_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief FMAC get data callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_GetDataCallback(FMAC_HandleTypeDef *hfmac)
{
  GetDataCallbackCount++;

  if (GetDataCallbackCount == 1)
  {
    /* Write the following input values into FMAC (2 over 3) */
    CurrentInputArraySize = INPUT_ARRAY_2_SIZE;
    if (HAL_FMAC_AppendFilterData(hfmac,
                                  aInputValues2,
                                  &CurrentInputArraySize) != HAL_OK)
    {
      ErrorCount++;
    }
  }
  else if (GetDataCallbackCount == 2)
  {
    /* Write the following input values into FMAC (3 over 3) */
    CurrentInputArraySize = INPUT_ARRAY_3_SIZE;
    if (HAL_FMAC_AppendFilterData(hfmac,
                                  aInputValues3,
                                  &CurrentInputArraySize) != HAL_OK)
    {
      ErrorCount++;
    }
  }
  else
  {
    /* No more data will be written, disable the FMAC write interrupt */
    __HAL_FMAC_DISABLE_IT(hfmac, FMAC_IT_WIEN);
  }
}

/**
  * @brief FMAC error callback
  * @par hfmac: FMAC HAL handle
  * @retval None
  */
void HAL_FMAC_ErrorCallback(FMAC_HandleTypeDef *hfmac)
{
  ErrorCount++;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
    /* LED2 is blinking */
    BSP_LED_Toggle(LED2);
    HAL_Delay(500);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
