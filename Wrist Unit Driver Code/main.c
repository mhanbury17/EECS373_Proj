/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
/* ========================================== Includes ========================================= */ // includes begin
#include "Adafruit_ILI9341.h"
#include "Adafruit_STMPE610.h"

/* ============================================================================================= */ // includes end
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* ===================================== Global Variables ====================================== */ // global variables begin
static TSPoint point;

SPI_HandleTypeDef* ILI9341_HSPI_INST = &hspi1;                                                      // hspi instance pointer
I2C_HandleTypeDef* STMPE610_HI2C_INST = &hi2c1;

GPIO_TypeDef* ILI9341_CSX_PORT = GPIOA;                                                             // csx pin location
uint16_t ILI9341_CSX_PIN  = GPIO_PIN_4;

GPIO_TypeDef* ILI9341_DCX_PORT = GPIOA;                                                             // dcx pin location
uint16_t ILI9341_DCX_PIN  = GPIO_PIN_1;

/* ================================= Function Implementations ================================== */ // global variables end
void DataHandler(uint8_t* data, cursor_t* cur)
{
    HAL_UART_Receive(&huart2, &data[0], 100, 1000);                                                 // receives data from rasberry pi speech recognition

    if (data[0] >= ' ' && data[0] <= 'z')                                                           // checks that string is valid
    {
        __disable_irq();
        ILI9341_PrintString(cur, (char*)data);                                                      // prints string
        for (uint8_t i = 0; i < 100; ++i) data[i] = '\0';                                           // clear buffer
        __enable_irq();
    }
    else if (data[0] == '{')
    {
        cursor_t arrow_cur = {ILI9341_WIDTH - ILI9341_ARROW_BASE_WIDTH*ILI9341_GetArrowSize() - 10, 4};

        switch ((int)(data[1]))
        {
        case 0: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_E, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 1: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_NE, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 2: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_N, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 3: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_NW, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 4: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_W, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 5: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_SW, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 6: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_S, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        case 7: ILI9341_PrintArr16(&arrow_cur, ILI9341_ARROW_SE, ILI9341_ARROW_BASE_WIDTH, ILI9341_GetArrowSize()); break;
        
        default: /* do nothing */ break;
        }
    }
}

screen_enum Homescreen_TouchHandler(cursor_t* cur)
{
    if (STMPE610_TouchedArea(point, 20 + ILI9341_BLOCKM_BASE_WIDTH, 20))                            // if user touches settings icon
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        ILI9341_SetupSettingsInterface();
        __enable_irq();
        return SETTINGS;
    }
    else if (STMPE610_TouchedArea(point, ILI9341_WIDTH - 5*(ILI9341_FONT_BASE_WIDTH + 1), 20))      // if user touches clear button
    {
        __disable_irq();
        ILI9341_ResetTextBox(cur);
        STMPE610_ClearPoint(&point);
        __enable_irq();
    }
    return HOMESCREEN;
}

screen_enum Settings_TouchHandler(cursor_t* cur)
{
    int changedBrightness = 0;                                                              // boolean var if brightness has been changed
    
    if(STMPE610_TouchedArea(point, 171, 202))                                               // If user trying to increase Font Size
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if(ILI9341_GetFontSize() < 8)
        {
            ILI9341_SetFontParam(ILI9341_GetFontSize() + 1);
            ILI9341_AdjustSlider(ILI9341_GetFontSize(), 145, 1);
        }
        __enable_irq();
    }
    else if(STMPE610_TouchedArea(point, 171, 18))                                           // If user trying to decrease Font Size
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if(ILI9341_GetFontSize() > 1)
        {
            ILI9341_SetFontParam(ILI9341_GetFontSize() - 1);
            ILI9341_AdjustSlider(ILI9341_GetFontSize(), 145, 0);
        }
        __enable_irq();
    }
    else if(STMPE610_TouchedArea(point, 286, 202))                                          // If user trying to increase arrow Size
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if(ILI9341_GetArrowSize() < 8)
        {
            ILI9341_SetArrowParam(ILI9341_GetArrowSize() + 1);
            ILI9341_AdjustSlider(ILI9341_GetArrowSize(), 260, 1);
        }
        __enable_irq();
    }
    else if(STMPE610_TouchedArea(point, 286, 18))                                           // If user trying to decrease arrow Size
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if(ILI9341_GetArrowSize() > 0)
        {
            ILI9341_SetArrowParam(ILI9341_GetArrowSize() - 1);
            ILI9341_AdjustSlider(ILI9341_GetArrowSize(), 260, 0);
        }
        __enable_irq();
    }
    else if(STMPE610_TouchedArea(point, 56, 202))                                           // If user trying to increase brightness
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if(ILI9341_GetBrightness() < 8)
        {
            ILI9341_SetBrightness(ILI9341_GetBrightness() + 1);
            ILI9341_AdjustSlider(ILI9341_GetBrightness(), 30, 1);
            changedBrightness = 1;
        }
        __enable_irq();
    }
    else if(STMPE610_TouchedArea(point, 56, 18))                                            // If user trying to decrease brightness
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if(ILI9341_GetBrightness() > 1)
        {
            ILI9341_SetBrightness(ILI9341_GetBrightness() - 1);
            ILI9341_AdjustSlider(ILI9341_GetBrightness(), 30, 0);
            changedBrightness = 1;
        }
        __enable_irq();
    }
    else if(STMPE610_TouchedArea(point, 20, 220))                                           // If user pressed return
    {
        __disable_irq();
        STMPE610_ClearPoint(&point);
        if (changedBrightness)
        {
            ILI9341_UpdateColor();
            changedBrightness = 0;
        }
        ILI9341_SetupSTTInterface();
        ILI9341_ResetTextBox(cur);
        __enable_irq();
        return HOMESCREEN;
    }
    return SETTINGS;
}

/* ============================================================================================= */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */
    /* USER CODE END 1 */
    
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_LPUART1_UART_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */
    /* ========================================== Setup ======================================== */ // setup begin
    /* ----------------------------------- Initialize Devices ---------------------------------- */
    ILI9341_Init();                                                                                 // initializes the display
    STMPE610_Init();                                                                                // initializes the touchscreen
    
    /* ---------------------------------- Initialize Variables --------------------------------- */
    cursor_t cur;                                                                                   // initialize text cursor

    HAL_TIM_Base_Start_IT(&htim2);                                                                  // initialize timer for touchscreen
    STMPE610_ClearPoint(&point);                                                                    // initialize touchscreen point
    
    uint8_t data[100];                                                                              // read data buffer
    for (uint8_t i = 0; i < 100; ++i) data[i] = '\0';                                               // clear data buffer
    
    /* ---------------------------------------- Setup UI --------------------------------------- */
    screen_enum curScreen = HOMESCREEN;                                                             // initialize UI menu
    ILI9341_SetupSTTInterface();                                                                    // setup speech-to-text interface
    ILI9341_ResetTextBox(&cur);                                                                     // reset the text box

    /* ========================================================================================= */ // setup end
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* ======================================= Loop ======================================== */ // loop begin
        switch (curScreen)
        {
        case HOMESCREEN:
            DataHandler(data, &cur);
            curScreen = Homescreen_TouchHandler(&cur);
            break;

        case SETTINGS:
            curScreen = Settings_TouchHandler(&cur);
            break;

        }
    /* ========================================================================================= */ // loop end
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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure the main internal regulator output voltage
      */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
      * in the RCC_OscInitTypeDef structure.
      */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
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

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_LPUART1
                                |RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
    /* USER CODE BEGIN I2C1_Init 0 */
    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */
    /* USER CODE END I2C1_Init 1 */

    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00707CBB;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN I2C1_Init 2 */
    /* USER CODE END I2C1_Init 2 */
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{
    /* USER CODE BEGIN LPUART1_Init 0 */
    /* USER CODE END LPUART1_Init 0 */

    /* USER CODE BEGIN LPUART1_Init 1 */
    /* USER CODE END LPUART1_Init 1 */

    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 209700;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_7B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_NONE;
    hlpuart1.Init.Mode = UART_MODE_TX_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_HalfDuplex_Init(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN LPUART1_Init 2 */

    /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
    /* USER CODE BEGIN USART2_Init 0 */
    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */
    /* USER CODE END USART2_Init 1 */

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* USER CODE BEGIN USART2_Init 2 */
    /* USER CODE END USART2_Init 2 */
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
    /* USER CODE BEGIN SPI1_Init 0 */
    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */
    /* USER CODE END SPI1_Init 1 */

    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* USER CODE BEGIN SPI1_Init 2 */
    /* USER CODE END SPI1_Init 2 */
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
    /* USER CODE BEGIN TIM2_Init 0 */
    /* USER CODE END TIM2_Init 0 */

    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */
    /* USER CODE END TIM2_Init 1 */

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 3199;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
    sSlaveConfig.InputTrigger = TIM_TS_ITR0;
    if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN TIM2_Init 2 */
    /* USER CODE END TIM2_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : PA1 PA4 */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : LD3_Pin */
    GPIO_InitStruct.Pin = LD3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/* ===================================== Interrupt Handler ===================================== */
// Callback: timer has rolled over
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Check which version of the timer triggered this callback and toggle LED
    if (htim == &htim2 )
    {
        if(STMPE610_Touched())
        {
            point = STMPE610_GetPoint();
        }
    }
}
/* ============================================================================================= */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
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
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

