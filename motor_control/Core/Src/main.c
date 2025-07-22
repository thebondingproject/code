/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file  main.c
 * @brief Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/** @todo These are here for debug, they should be moved somewhere else at a latter stage */
float maxacc = 0.1;
float maxvel = 2;
/** @todo These are here for debug, they should be moved somewhere else at a latter stage */
uint32_t received_data;
uint8_t received_address;

uint32_t count;

uint32_t ADC_value;

float di_dt = 0, t_decay = 0, I_peak = 0;

/*
 * 0: position control mode (default)
 * 1: speed control mode
 */
uint32_t control_mode = POS_MODE;

float setpoint_current_vel, measured_speed_old;
float setpoint_speed_in;
uint32_t time_at_min_halt;
uint32_t time_at_max_halt;
uint32_t  last_time_A = 0, last_time_B = 0;
uint8_t  last_state_A = 0, last_state_B = 0;
float Halt_command = 1.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
uint8_t is_switch_pressed_debounced(GPIO_TypeDef* port, uint16_t pin, uint8_t *last_state, uint32_t *last_time);
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
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	// Starts the motor encoders
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_2);
	// Starts the timers for PWM generation
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

	// Global variables initialization
	serial_communication_variable_init();
	controller_variables_init();

	// Allows for the motor controller to properly initialize at startup
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	// See p. 10 of the MAX22201.pdf datasheet. We need to give time to the motor driver to properly startup
	HAL_Delay(1);

	// Check if the motor has already engaged a limit switch
	if (HAL_GPIO_ReadPin(LIMB_GPIO_Port, LIMB_Pin)) {
		motor_halt_max = 1;
	}
	if (HAL_GPIO_ReadPin(LIMA_GPIO_Port, LIMA_Pin)) {
		motor_halt_min = 1;
	}

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

	// WATCH OUT: this must be ALWAYS put AFTER the motor controller initialization delay!!!
	HAL_ADC_Start_IT(&hadc1);

	// Properly initializes the motion_profile function parameters
	target_position = 0;
	position_at_call = measured_position;
	time_at_call = HAL_GetTick();

	// All initializations are okay. Turn on the ALIVE status LED
	HAL_GPIO_WritePin(ALIVE_LED_GPIO_Port, ALIVE_LED_Pin, GPIO_PIN_SET);

	HAL_I2C_EnableListen_IT(&hi2c1);

	//	target_position = -0.8f;
	//	position_at_call = measured_position;
	//	time_at_call = HAL_GetTick();
	//
	//	HAL_Delay(20000);
	//
	//	target_position = -0.1f;
	//		position_at_call = measured_position;
	//		time_at_call = HAL_GetTick();

	HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#if 1
		/*
		 * Resets the motor_halt flag which prevents the motor from moving further when the limit switches are disengaged.
		 * (see also HAL_GPIO_EXTI_Callback)
		 *
		 * The motor start moving when we command a new velocity in the opposite
		 * direction wrt the location of the limit switch.
		 */
		// POBLEM: switch bounces when disengaged
		if (motor_halt_max == 1 && is_switch_pressed_debounced(LIMB_GPIO_Port, LIMB_Pin, &last_state_B, &last_time_B)) {
			motor_halt_max = 0;
		}
		if (motor_halt_min == 1 && is_switch_pressed_debounced(LIMA_GPIO_Port, LIMA_Pin, &last_state_A, &last_time_A)) {
			motor_halt_min = 0;
		}


		//		setpoint_position = -0.8;
		//		HAL_Delay(3000);
		//		setpoint_position = -0.1;
		//		HAL_Delay(3000);

		//HAL_Delay(10);
#else
		target_position = 50;
		position_at_call = measured_position;
		time_at_call = HAL_GetTick();
		HAL_Delay(7000);
		target_position = -50;
		position_at_call = measured_position;
		time_at_call = HAL_GetTick();
		HAL_Delay(7000);
		target_position = 50;
		position_at_call = measured_position;
		time_at_call = HAL_GetTick();
		HAL_Delay(7000);
		target_position = 100;
		position_at_call = measured_position;
		time_at_call = HAL_GetTick();
		HAL_Delay(7000);
		target_position = 0;
		position_at_call = measured_position;
		time_at_call = HAL_GetTick();
		HAL_Delay(7000);
#endif
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_ADC1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Adc1ClockSelection = RCC_ADC1PLLCLK_DIV1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_TRGO;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = SLAVE_ADDRESS;
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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
  htim1.Init.Period = 1023;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC3REF;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 2;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, ALIVE_LED_Pin|COM_LED_Pin|FAULT_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : FAULTB_Pin FAULTA_Pin */
  GPIO_InitStruct.Pin = FAULTB_Pin|FAULTA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DEBUG_OUT_Pin */
  GPIO_InitStruct.Pin = DEBUG_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DEBUG_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA10 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ALIVE_LED_Pin COM_LED_Pin */
  GPIO_InitStruct.Pin = ALIVE_LED_Pin|COM_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : FAULT_LED_Pin */
  GPIO_InitStruct.Pin = FAULT_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(FAULT_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LIMA_Pin LIMB_Pin */
  GPIO_InitStruct.Pin = LIMA_Pin|LIMB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int execute_read_command(uint8_t read_address)
{
	int ret_val = COMM_ERR;
	ui32_to_f conv;
	uint16_t packet_size;

	switch (read_address)
	{
	case VIRTUAL_MEM_POSITION_MEAS:
		conv.f = measured_position;
		packet_size = prepare_WRITE_packet(tx_buffer, read_address, conv.ui32);
		// Sends the read data back to the master
		//HAL_I2C_Slave_Transmit_DMA(&hi2c1, tx_buffer, MAX_COMM_BUFFER_SIZE);
		//HAL_I2C_Slave_Transmit_IT(&hi2c1, tx_buffer, MAX_COMM_BUFFER_SIZE);
		HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, tx_buffer, MAX_COMM_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME);
		ret_val = COMM_OK;
		break;
	case VIRTUAL_MEM_SPEED_MEAS:
		conv.f = measured_speed;
		packet_size = prepare_WRITE_packet(tx_buffer, read_address, conv.ui32);
		// Sends the read data back to the master
		//HAL_I2C_Slave_Transmit_DMA(&hi2c1, tx_buffer, MAX_COMM_BUFFER_SIZE);
		//HAL_I2C_Slave_Transmit_IT(&hi2c1, tx_buffer, MAX_COMM_BUFFER_SIZE);
		HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, tx_buffer, MAX_COMM_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME);
		ret_val = COMM_OK;
		break;
	default:
		ret_val = COMM_ERR;
		break;
	}

	return ret_val;
}

int execute_write_command(uint8_t write_address, uint32_t data)
{
	int ret_val = COMM_ERR;
	ui32_to_f conv;
	conv.ui32 = data;

	switch (write_address)
	{
	case VIRTUAL_MEM_POSITION:
		/*
		 * Refuse to update the target position until we have completed the current trajectory.
		 * This prevents janky motor movements when master updates the position faster than
		 * the motor can keep up with.
		 */
		/** @todo set proper precision */

		//if (fabs(target_position - measured_position) < 0.01 && control_mode == FULL_MODE)
		//{
		target_position = conv.f;
		position_at_call = measured_position;
		time_at_call = HAL_GetTick();
		//}
		ret_val = COMM_OK;
		break;

	case VIRTUAL_MEM_SPEED:
		setpoint_speed = conv.f;
		ret_val = COMM_OK;
		break;

	case VIRTUAL_MEM_MODE:
		control_mode = data;
//		if(data == POS_MODE) {
//			target_position = measured_position;
//			position_at_call = measured_position;
//			time_at_call = HAL_GetTick();
//		}
		if(data == SPD_MODE)
			setpoint_speed = 0;
		ret_val = COMM_OK;
		break;

	default:
		ret_val = COMM_ERR;
		break;
	}

	return ret_val;
}

void HAL_I2C_SlaveRxCpltCallback (I2C_HandleTypeDef *hi2c1)
{
	HAL_GPIO_TogglePin(COM_LED_GPIO_Port, COM_LED_Pin);
	//Perform different actions based on the received command
	switch (rx_buffer[0])
	{
	case COMMAND_WRITE:
		read_data_from_WRITE_command(rx_buffer, &received_address, &received_data);
		execute_write_command(received_address, received_data);
		// Restore the halt mode
		Halt_command = 1.0f;
		/* No answer for now
		 uint16_t packet_size;
		 if(execute_write_command(received_address, received_data) == COMM_OK)
		 packet_size = prepare_ACK_packet(tx_buffer);
		 else
		 packet_size = prepare_NACK_packet(tx_buffer);
		 HAL_I2C_Slave_Transmit(&hi2c1, tx_buffer, packet_size, 10);
		 */
		break;
	case COMMAND_READ:
		received_address = rx_buffer[2];
		//execute_read_command(received_address);
		break;
	case COMMAND_STOP:
		// halt the motor
		Halt_command = 0.0f;
		break;
	}

	return;
}

/** @brief Converts the encoder AB data to a motor position in radians */
float read_position_from_encoder(void)
{
	/*
	 * __HAL_TIM_GET_COUNTER(&htim2)  = number of counted encoder impulses
	 * (2 * 3.14f / (PPR_ENCODER * 4) = impulses per revolution in 4x mode, converted into radians
	 *      PPR_ENCODER: base number of impulses per revolution
	 * 		* 4 : encoder is in x4 mode (we count both the rising and falling edges)
	 * 		2*pi: converts into radians
	 */
	return (int32_t) (__HAL_TIM_GET_COUNTER(&htim2)) * (2 * 3.14f / (PPR_ENCODER * 4));
}

/**
 * @brief Handles the GPIO callback
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == LIMA_Pin)
	{
		//		// Saves the last position when the limit switch sensor is reached
		//		last_position_min = read_position_from_encoder();
		//		setpoint_position = last_position_min;
		//		// Halts the motor immediately
		//		motor_halt_min = true;

		// When limit is reached force the velocity to 0 and save the current position
		// For debounce the halt flag is cleared in software after xx ms
		if(motor_halt_min == 0) {
			motor_halt_min = 1;
			time_at_max_halt = HAL_GetTick();
			last_position_min = read_position_from_encoder();
		}
	}

	if (GPIO_Pin == LIMB_Pin)
	{
		//		last_position_max = read_position_from_encoder();
		//		setpoint_position = last_position_max;
		//		motor_halt_max = true;

		if(motor_halt_max == 0) {
			motor_halt_max = 1;
			time_at_min_halt = HAL_GetTick();
			last_position_max = read_position_from_encoder();
		}
	}

	if (GPIO_Pin == FAULTA_Pin)
	{
		Error_Handler();
	}

	if (GPIO_Pin == FAULTB_Pin)
	{
		Error_Handler();
	}
}

/**
 * @brief Handles the ADC end of conversion callback
 *
 * @param hadc ADC handler object
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
	bool direction;
	//uint32_t ADC_value;

	//Per prima cosa calcoliamo la corrente nel motore
	ADC_value = HAL_ADC_GetValue(hadc);
	/** Converts the measured ADC_value into a current in Amperes
	 *  - 3.3f: the fullscale voltage of the ADC is 3.3V
	 *  - 4095.0f: we have a 12 bit ADC
	 *  - 5.0f: is the internal gain of the amplifier inside the motor driver
	 *  - 0.1f: is the value of the shunt resistor
	 */
	//measured_current = ADC_value * 3.3f/4095.0f/5.0f/0.1f;
	measured_current = ADC_value * 3.3f/4095.0f/0.1f;


	/* Position control loop */
	//Eseguo il loop di velocità ogni tot iterazioni del loop di corrente
	interrupt_counter++;

	if (interrupt_counter % 10 == 0)
	{
		if (control_mode == POS_MODE)
		{
			//Motion Profiling
			setpoint_position = motion_profile(maxacc, maxvel, target_position, position_at_call, HAL_GetTick(), time_at_call);
			// Salvo posizione
			measured_position = read_position_from_encoder();

			setpoint_speed = PI_controller(setpoint_position, measured_position, &loop_parameters_position);
		}
		//If in SPD_MODE, speed gets set by execute_write_command
	}

	/* Speed control loop */
	if (interrupt_counter % 5 == 0)
	{
		if (control_mode == POS_MODE || control_mode == SPD_MODE)
		{
			measured_position = (int32_t) (__HAL_TIM_GET_COUNTER(&htim2)) * (2 * 3.14f / (PPR_ENCODER * 4));

			// calculation in rad/sec
			measured_speed = (measured_position - old_position) / loop_parameters_speed.T;

			if(motor_halt_min){
				if(setpoint_speed > 0)
					setpoint_speed_in = setpoint_speed;
				else
					setpoint_speed_in = 0;
			}
			else if(motor_halt_max){
				if(setpoint_speed < 0)
					setpoint_speed_in = setpoint_speed;
				else
					setpoint_speed_in = 0;
			}
			else
			{
				setpoint_speed_in = setpoint_speed;
			}

			setpoint_speed_in = setpoint_speed_in * Halt_command;

			// Filtro velocity
			filtered_speed = ALPHA * measured_speed + (1 - ALPHA) * filtered_speed;

			// Filtro velocity
			//filtered_speed = 0.3758f * measured_speed + 0.3758f * measured_speed_old + 0.2484f * filtered_speed;

			measured_speed_old = measured_speed;

			setpoint_current_vel = PI_controller(setpoint_speed_in, filtered_speed, &loop_parameters_speed);

			old_position = measured_position;
		}
	}

	// Aggiungo feedforward di coppia
	setpoint_current  = setpoint_current_vel + spring_compensation_current + setpoint_position*K_SPRING;

	if (setpoint_current > 0)
		direction = 1;
	else if (setpoint_current < 0)
		direction = 0;

	// Se il valore pwm precedente è zero, calcola la corrente dal modello utilizzando la corrente precedentemente
	// misurata dal ADC

	if((output_current / 12.0f * 1023) <= 20)
	{
		measured_current = I_peak * exp(-(R / L) * t_decay);
		t_decay += loop_parameters_current.T * (1 - output_current / 12.0f);
	}
	else
	{
		t_decay = loop_parameters_current.T * (1 - output_current / 12.0f);

		// 2. Compute current slope during high-side ON
		//float di_dt = (V_bus - V_emf - I_mid * R) / L;
		di_dt = (12 - measured_current * R) / L;

		// 3. Estimate peak current at end of high-side ON
		I_peak = measured_current + di_dt * (output_current / 12.0f * loop_parameters_current.T / 2);
		//I_peak = measured_current;
	}

	/* Current control loop */
	// Parametri calcolati per 12volt output
	// Per funzionare la corrente deve essere scalata al valore pwm
	output_current = PI_controller(fabs(setpoint_current), measured_current, &loop_parameters_current);

	/*
	 * Halts the motor (i.e., sets current target to 0) whenever a limit switch
	 * is reached (i.e., when either motor_halt_max or motor_halt_min becomes true).
	 */
	//if (!(motor_halt_max || motor_halt_min))
	if (true)
	{
		// pilotaggio pwm
		if (direction)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (int16_t )(output_current / 12.0f * 1023));
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
		}
		else
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (int16_t )(output_current / 12.0f * 1023));
		}
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	}
	HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
}


void HAL_I2C_ListenCpltCallback (I2C_HandleTypeDef *hi2c)
{
	// Re-enable i2c listen
	HAL_I2C_EnableListen_IT(hi2c);
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if(TransferDirection == I2C_DIRECTION_TRANSMIT)  // if the master wants to transmit the data
	{
		//HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rx_buffer, 32, I2C_FIRST_AND_LAST_FRAME);
		if(HAL_I2C_Slave_Sequential_Receive_IT(hi2c, rx_buffer, MAX_COMM_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME) != HAL_OK)
		{
			Error_Handler();
		}
		//HAL_I2C_Slave_Receive_DMA(hi2c, rx_buffer, MAX_COMM_BUFFER_SIZE);
	}
	else  // master requesting the data
	{
		execute_read_command(received_address);
	}
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{

}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	//HAL_I2C_EnableListen_IT(hi2c);
	HAL_GPIO_WritePin(FAULT_LED_GPIO_Port, FAULT_LED_Pin, GPIO_PIN_SET);

	//Reset the I2C interface.
	HAL_I2C_DeInit(&hi2c1);
	HAL_I2C_Init(&hi2c1);

	HAL_GPIO_WritePin(FAULT_LED_GPIO_Port, FAULT_LED_Pin, GPIO_PIN_RESET);
}

uint8_t is_switch_pressed_debounced(GPIO_TypeDef* port, uint16_t pin, uint8_t *last_state, uint32_t *last_time) {
	uint8_t current_state = HAL_GPIO_ReadPin(port, pin);
	uint32_t current_tick = HAL_GetTick();

	if (current_state != *last_state) {
		*last_time = current_tick;
		*last_state = current_state;
	}

	return ((current_tick - *last_time) >= 5) && !current_state;
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
	__disable_irq();
	while (1)
	{
		HAL_GPIO_TogglePin(FAULT_LED_GPIO_Port, FAULT_LED_Pin);
		HAL_Delay(1000);
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
