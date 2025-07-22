/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "serial_communication.h"
#include <math.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FIRST_SEARCH_HEIGHT 1
#define START_DESCEND1 2
#define BOND1 3
#define SECOND_SEARCH_HEIGHT 4
#define START_DESCEND2 5
#define BOND2 6
#define RESET_POSITION 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
/** Bonding parameters */
float search_height_1 = -0.5, search_height_2 = -0.5;
float further_descent = 0.01;
float loop_height = 0.5;
uint8_t bond_power = 50;
uint32_t bond_time_1 = 100, bond_time_2 = 100;
float search_height_adj_1 = 0;
float search_height_adj_2 = 0;

/** @todo These are here for debug, they should be moved somewhere else at a latter stage */
float measured_temperature_gauge = 0;
uint32_t debug_head_touched;
uint32_t stage = 0;
uint32_t received_data;
uint8_t received_address;
float read_position;
bool head_touched;
bool data_ready;
uint32_t temperature_testpoint;
float ADC_valX;
float ADC_valY;
float position_joystick = 0;
float speed_joystickX = 0;
float speed_joystickY = 0;
float speed_joystickZ = 0;
float speed_joystickT = 0;
float tmpX, tmpY;
float head_angle = 0;
uint32_t solenoid_tear_1_status = 0;
uint32_t solenoid_tear_2_status = 0;
uint32_t solenoid_tear_clamp_status = 0;
uint32_t solenoid_tear_lock_status = 0;
uint32_t solenoid_lock_status = 0, lamp_status = 0;
uint32_t fine_adjustment;
uint8_t bond_power_now, bond_power_before;

uint32_t ADC_raw[2];
volatile bool ADC_ready = 0;

uint8_t FSM_currentState = RESET_POSITION, FSM_nextState = 0;

/*
 * 0: XY
 * 1: z
 * 2: theta
 */
uint32_t control_mode = 0;
int32_t tst;

ADC_ChannelConfTypeDef ADC_CH_Cfg = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC3_Init(void);
/* USER CODE BEGIN PFP */
float ADC_Convert_Rank1 (void);
float ADC_Convert_Rank2 (void);
void write_bond_power_to_trimmer(uint8_t bond_power);
void wait_for_button_release(void);
void wait_for_button_press(void);
void perform_bond(uint32_t bond_time);
void write_float_to_slave(uint8_t slave_address, uint8_t virtual_memory_address, float value);
void write_uint32_to_slave(uint8_t slave_address, uint8_t virtual_memory_address, uint32_t value);
void enter_jog_mode(void);
void measure_joystick_and_send_to_slave(void);

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

	/* MPU Configuration--------------------------------------------------------*/
	MPU_Config();

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CRC_Init();
	MX_DMA2D_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_ADC2_Init();
	MX_ADC3_Init();
	/* USER CODE BEGIN 2 */
	serial_communication_variable_init();
	speed_joystickX = 0;
	speed_joystickY = 0;
	speed_joystickZ = 0;
	speed_joystickT = 0;
	solenoid_tear_1_status = 0;
	solenoid_tear_2_status = 0;
	solenoid_tear_clamp_status = 0;
	solenoid_tear_lock_status = 0;
	control_mode = 0;
	bond_power_now = 0;
	bond_power_before = 0;
	search_height_adj_1 = 0;
	search_height_adj_2 = 0;
	measured_temperature_gauge = 0;
	debug_head_touched = 0;
	fine_adjustment = 1;

	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
	{
		/* Starting Error */
		Error_Handler();
	}

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	position_joystick = 0;
	ui32_to_f conv;
	//https://deepbluembedded.com/stm32-potentiometer-read-examples-single-multiple-potentiometers/
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

	//Disable the ultrasonic driver
	HAL_GPIO_WritePin(ULTRASONIC_ENABLE_GPIO_Port, ULTRASONIC_ENABLE_Pin, GPIO_PIN_SET);

	//Reset position at startup
	write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, POS_MODE);
	HAL_Delay(10);
	write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, 0);
	HAL_Delay(10);

	write_uint32_to_slave(ADDRESS_MOTOR_X, VIRTUAL_MEM_MODE, SPD_MODE);
	HAL_Delay(10);
	write_float_to_slave(ADDRESS_MOTOR_X, VIRTUAL_MEM_SPEED, 0);
	HAL_Delay(10);

	write_uint32_to_slave(ADDRESS_MOTOR_Y, VIRTUAL_MEM_MODE, SPD_MODE);
	HAL_Delay(10);
	write_float_to_slave(ADDRESS_MOTOR_Y, VIRTUAL_MEM_SPEED, 0);
	HAL_Delay(10);

	write_uint32_to_slave(ADDRESS_MOTOR_T, VIRTUAL_MEM_MODE, SPD_MODE);
	HAL_Delay(10);
	write_float_to_slave(ADDRESS_MOTOR_T, VIRTUAL_MEM_SPEED, 0);
	HAL_Delay(10);

	while (1)
	{
//		write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, POS_MODE);
//		HAL_Delay(10);
//		write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, 0);
//		HAL_Delay(2500);
//		write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, POS_MODE);
//		HAL_Delay(10);
//		write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, -0.5);
//		HAL_Delay(2500);
//		write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, SPD_MODE);
//		HAL_Delay(10);
//		write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_SPEED, -0.5);
//		HAL_Delay(5000);

		/** Transmit the value of the bond power to the digital trimmer only when it changes */
		/** @todo TO REMOVE, this is only here for debug */
		//		if(bond_power_now != bond_power_before)
		//		{
		//			uint8_t payload[2] = {0x11, bond_power_now};
		//			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
		//			HAL_SPI_Transmit(&hspi1, payload, 2, 100);
		//			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
		//			bond_power_before = bond_power_now;
		//		}

		//		prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
		//		HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
		//		HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
		//
		//		HAL_Delay(100);
		//
		//		ui32_to_f descent_position;
		//
		//		descent_position.f = -0.8f;
		//		prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_POSITION, descent_position.ui32);
		//		HAL_I2C_Master_Transmit_IT(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE);
		//		HAL_Delay(6000);
		//				descent_position.f = -0.1f;
		//				prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_POSITION, descent_position.ui32);
		//				HAL_I2C_Master_Transmit_IT(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE);
		//				HAL_Delay(6000);

		//Keep the head raised
		debug_head_touched = HAL_GPIO_ReadPin(PLATFORM_TOUCH_GPIO_Port, PLATFORM_TOUCH_Pin);
//		HAL_Delay(100);
		//


		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		ADC_raw[0] = HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, 1000);
		ADC_raw[1] = HAL_ADC_GetValue(&hadc3);

		//Enable XY jog mode
		measure_joystick_and_send_to_slave();
		//HAL_Delay(100)


		//Write head position
		//		write_float_to_slave(ADDRESS_MOTOR_T, VIRTUAL_MEM_POSITION, head_angle);
#if 0
		tmpX = ADC_Convert_Rank1();
		tmpY = ADC_Convert_Rank2();
		ADC_valX = tmpX - 127.5-9.5;
		ADC_valY = tmpY - 127.5-9.5;         // Read The ADC Conversion Result
		/*
		 * Fine: /250.0
		 * Coarse: /50.0
		 */
		if (fine_adjustment)
			divider = 250.0;
		else
			divider = 50.0;

		switch (control_mode)
		{
		case 0:
			if (fabs(ADC_valX) > 5)
				speed_joystickX = ADC_valX;
			else
				speed_joystickX = 0;

			if (fabs(ADC_valY) > 5)
				speed_joystickY = ADC_valY;
			else
				speed_joystickY = 0;

			write_float_to_slave(ADDRESS_MOTOR_X, VIRTUAL_MEM_SPEED, speed_joystickX / divider);
			write_float_to_slave(ADDRESS_MOTOR_Y, VIRTUAL_MEM_SPEED, speed_joystickY / divider);
			break;
		case 1:
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_SPEED, speed_joystickZ / divider);
			break;
		case 2:
			write_float_to_slave(ADDRESS_MOTOR_T, VIRTUAL_MEM_SPEED, speed_joystickT / divider);
			break;
		}
		HAL_Delay(100);
#endif

#if 0
		if(HAL_GPIO_ReadPin(BUTTON_START_BOND_GPIO_Port, BUTTON_START_BOND_Pin) == GPIO_PIN_RESET && FSM_currentState != FSM_nextState)
		{
			FSM_currentState = FSM_nextState;
			// start bond sequence...
			// GO to First search height (settable)
			// Wait button press
			// First bond
			// Loop height
			// Wai button press
			// Second search height (settable)
			// Wait button
			// second bond
			// wire cut
			// Reset position
		}

		switch(FSM_currentState){
		case FIRST_SEARCH_HEIGHT:
			HAL_GPIO_WritePin(SOLENOID_LOCK_GPIO_Port, SOLENOID_LOCK_Pin, GPIO_PIN_SET);
			HAL_Delay(200);
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, search_height_1);
			HAL_Delay(5000);
//			prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
//			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
//			HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
//			HAL_Delay(100);
//			// Se non va sostituire con delay

			/** @todo mettere una tolleranza seria anziché 0.01, o perlomeno metterla in un define */
			//if(fabs(read_position-search_height_1)<0.01)
			FSM_nextState = START_DESCEND1;
			break;
		case START_DESCEND1:
			HAL_GPIO_WritePin(SOLENOID_LOCK_GPIO_Port, SOLENOID_LOCK_Pin, GPIO_PIN_RESET);
			HAL_Delay(200);
			//write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, -1);
			write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, SPD_MODE);
			HAL_Delay(10);
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_SPEED, 1);
			HAL_Delay(10);
			FSM_currentState = BOND1;
			FSM_nextState = BOND1;
			break;
		case BOND1:
			// Posso bloccare il resto mentre faccio primo bond
			HAL_Delay(200);
			prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
			if(head_touched){
				write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, read_position - further_descent);
				HAL_Delay(1000);
				perform_bond(bond_time_1);
				// Open clamp solenoid
				HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_SET);
				//Go to loop height
				write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, POS_MODE);
				HAL_Delay(10);
				write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, (read_position - further_descent) + loop_height);
				HAL_Delay(10);
				FSM_nextState = SECOND_SEARCH_HEIGHT;
			}
			head_touched = false;
			HAL_Delay(100);
			break;
		case SECOND_SEARCH_HEIGHT:
			HAL_GPIO_WritePin(SOLENOID_LOCK_GPIO_Port, SOLENOID_LOCK_Pin, GPIO_PIN_SET);
			HAL_Delay(200);
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, search_height_2);
			HAL_Delay(4000);
			//prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
			//HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			//HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);

			//if(read_position == search_height_2)
			FSM_nextState = START_DESCEND2;
			break;
		case START_DESCEND2:
			HAL_GPIO_WritePin(SOLENOID_LOCK_GPIO_Port, SOLENOID_LOCK_Pin, GPIO_PIN_RESET);
			HAL_Delay(200);
			//write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, -1);
			write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, SPD_MODE);
			HAL_Delay(10);
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_SPEED, 0.1);
			HAL_Delay(10);
			FSM_currentState = BOND2;
			FSM_nextState = BOND2;
			break;

		case BOND2:
			// Posso bloccare il resto mentre faccio primo bond
			prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
			if(head_touched){
				write_uint32_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_MODE, POS_MODE);
				write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, read_position - further_descent);
				HAL_Delay(1000);
				perform_bond(bond_time_2);
				HAL_GPIO_WritePin(SOLENOID_TEAR_2_GPIO_Port, SOLENOID_TEAR_2_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_SET);
				HAL_Delay(1000);

				// Close clamp solenoid
				HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_RESET);
				// Strappo
				HAL_GPIO_WritePin(SOLENOID_TEAR_2_GPIO_Port, SOLENOID_TEAR_2_Pin, GPIO_PIN_RESET);

				// Go back up
				write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, 0);

				HAL_Delay(1000);

				FSM_nextState = RESET_POSITION;
			}
			head_touched = false;
			HAL_Delay(100);
			break;
		case RESET_POSITION:
			HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_RESET);

			FSM_nextState = FIRST_SEARCH_HEIGHT;
			break;
		default:
			break;

		}
#endif

		// Execute this code


#if 1
		if(HAL_GPIO_ReadPin(BUTTON_START_BOND_GPIO_Port, BUTTON_START_BOND_Pin) == GPIO_PIN_RESET)
		{

			fine_adjustment = 1;

			stage = 0; //For debug; TODO: remove in the future
			// Wait for the user to release the button
			wait_for_button_release();

			ui32_to_f descent_position;

			stage++; //For debug; TODO: remove
			// First search descent
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, search_height_1);

			// Wait for the button to be pressed and released
			//wait_for_button_press();

#if 1
			stage++; //For debug; TODO: remove
			// Enter into jog mode (XY movement with joystick)
			enter_jog_mode();
			// Wait for the user to release the button
			wait_for_button_release();
#else
			wait_for_button_press();
#endif

			stage++; //For debug; TODO: remove
			//Start descending until touch with substrate
			head_touched = false;
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, -1);
			while(!head_touched){__NOP();};

			stage++; //For debug; TODO: remove
			//Lower the head a bit more
			data_ready = false;
			prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
			while(!data_ready){__NOP();};
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, read_position - further_descent);

			stage++; //For debug; TODO: remove
			//Perform the bond
			perform_bond(bond_time_1);

			// Open clamp solenoid
			HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_SET);

			//Go to loop height
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, (read_position - further_descent) + loop_height);

#if 1
			stage++; //For debug; TODO: remove
			// Enter into jog mode (XY movement with joystick)
			enter_jog_mode();
			// Wait for the user to release the button
			wait_for_button_release();
#else
			wait_for_button_press();
#endif
			//Go to search height 2
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, search_height_2);

			// Wait for the button to be pressed and released
			//wait_for_button_press();

#if 1
			stage++; //For debug; TODO: remove
			// Enter into jog mode (XY movement with joystick)
			enter_jog_mode();
			// Wait for the user to release the button
			wait_for_button_release();
#else
			wait_for_button_press();
#endif


			//Start descending until touch with substrate
			head_touched = false;
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, -1);
			while(!head_touched){__NOP();};

			stage++; //For debug; TODO: remove
			//Lower the head a bit more
			data_ready = false;
			prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
			while(!data_ready){__NOP();};
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, read_position - further_descent);

			stage++; //For debug; TODO: remove
			//Perform the bond
			perform_bond(bond_time_2);
			HAL_GPIO_WritePin(SOLENOID_TEAR_2_GPIO_Port, SOLENOID_TEAR_2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_SET);

			HAL_Delay(1000);

			stage++; //For debug; TODO: remove
			// Close clamp solenoid
			HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SOLENOID_TEAR_2_GPIO_Port, SOLENOID_TEAR_2_Pin, GPIO_PIN_RESET);

			stage++; //For debug; TODO: remove
			// Go back up
			head_touched = false;
			write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_POSITION, 0);

			HAL_Delay(1000);

			HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_RESET);

			fine_adjustment = 0;
		}
#endif

#if 0
		if(solenoid_tear_1_status)
			HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_RESET);

		if(solenoid_tear_2_status)
			HAL_GPIO_WritePin(SOLENOID_TEAR_2_GPIO_Port, SOLENOID_TEAR_2_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SOLENOID_TEAR_2_GPIO_Port, SOLENOID_TEAR_2_Pin, GPIO_PIN_RESET);

		if(solenoid_tear_clamp_status)
			HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SOLENOID_CLAMP_GPIO_Port, SOLENOID_CLAMP_Pin, GPIO_PIN_RESET);

		if(solenoid_lock_status)
			HAL_GPIO_WritePin(SOLENOID_LOCK_GPIO_Port, SOLENOID_LOCK_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SOLENOID_LOCK_GPIO_Port, SOLENOID_LOCK_Pin, GPIO_PIN_RESET);

		if(lamp_status)
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, GPIO_PIN_RESET);
		//https://deepbluembedded.com/stm32-potentiometer-read-examples-single-multiple-potentiometers/
		//		HAL_ADC_Start(&hadc1);
		//		HAL_ADC_PollForConversion(&hadc1, 10);
		//		ADC_valX = HAL_ADC_GetValue(&hadc1) - 127.5;


		//ADC_valX = ADC_Convert_Rank1() - 127.5;
		//ADC_valY = ADC_Convert_Rank2() - 127.5;         // Read The ADC Conversion Result

#if 0
#if SLAVE_CONTROL_MODE == POS_MODE
		if(fabs(ADC_val)>5)
		{
			//position_joystick += pow(ADC_val/100.0, 7);
			position_joystick += ADC_val/100.0;
			//position_joystick += ADC_val/500.0;
		}
		/* This needs to stay outside the if, otherwise we get janky movement when changing motor direction*/
		conv.f = position_joystick;
		prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_POSITION, conv.ui32);
		HAL_I2C_Master_Transmit_IT(&hi2c4, 2 * ADDRESS_MOTOR_X, tx_buffer, MAX_COMM_BUFFER_SIZE);

#endif
#if SLAVE_CONTROL_MODE == SPD_MODE
		/*
		 * Fine: /250.0
		 * Coarse: /50.0
		 */
		if(fine_adjustment)
			divider = 250.0;
		else
			divider = 50.0;

		switch(control_mode)
		{
		case 0:
#if 0
			if (fabs(ADC_valX) > 5)
				speed_joystickX = ADC_valX;
			else
				speed_joystickX = 0;
#endif
			conv.f = speed_joystickX/divider;
			tst=conv.ui32;
			prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_SPEED, conv.ui32);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_X, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);

#if 0
			if (fabs(ADC_valY) > 5)
				speed_joystickY = ADC_valY;
			else
				speed_joystickY = 0;
#endif
			conv.f = speed_joystickY/divider;
			tst=conv.ui32;
			prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_SPEED, conv.ui32);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Y, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
#endif
			break;
		case 1:
			conv.f = speed_joystickZ/divider;
			tst=conv.ui32;
			prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_SPEED, conv.ui32);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			break;
		case 2:
			conv.f = speed_joystickT/divider;
			tst=conv.ui32;
			prepare_WRITE_packet(tx_buffer, VIRTUAL_MEM_SPEED, conv.ui32);
			HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_T, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
			break;
		}


		HAL_ADC_Stop(&hadc1);
		HAL_Delay(100);
#endif
		//FOR DEBUG: Measure the position of the motor
		//prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
		//HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_X, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
		//HAL_I2C_Master_Receive(&hi2c1, ADDRESS_MOTOR_X, rx_buffer, MAX_COMM_BUFFER_SIZE, 1000);
		//HAL_I2C_Master_Seq_Receive_IT(&hi2c1, ADDRESS_MOTOR_X, rx_buffer, 32, I2C_FIRST_AND_LAST_FRAME);
#endif
		//Lettura verso lo slave
		//prepare_READ_packet(tx_buffer, VIRTUAL_MEM_POSITION_MEAS);
		//HAL_I2C_Master_Transmit(&hi2c1, ADDRESS_MOTOR_Z, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
		//HAL_I2C_Master_Receive_IT(&hi2c1, ADDRESS_MOTOR_Z, rx_buffer, MAX_COMM_BUFFER_SIZE);
		//Altre funzioni (che non servono)
		//HAL_I2C_Master_Receive(&hi2c1, ADDRESS_MOTOR_X, rx_buffer, MAX_COMM_BUFFER_SIZE, 1000);
		//HAL_I2C_Master_Seq_Receive_IT(&hi2c1, ADDRESS_MOTOR_X, rx_buffer, MAX_COMM_BUFFER_SIZE, I2C_FIRST_AND_LAST_FRAME);

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_Delay(100);
		//HAL_ADC_Start_IT(&hadc1);
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

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = 64;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 12;
	RCC_OscInitStruct.PLL.PLLP = 1;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
			|RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInitStruct.PLL2.PLL2M = 4;
	PeriphClkInitStruct.PLL2.PLL2N = 12;
	PeriphClkInitStruct.PLL2.PLL2P = 2;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 4096;
	PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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

	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_8B;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.Oversampling.Ratio = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief ADC2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC2_Init(void)
{

	/* USER CODE BEGIN ADC2_Init 0 */

	/* USER CODE END ADC2_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC2_Init 1 */

	/* USER CODE END ADC2_Init 1 */

	/** Common config
	 */
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
	hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2.Init.LowPowerAutoWait = DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.NbrOfConversion = 1;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc2.Init.OversamplingMode = DISABLE;
	hadc2.Init.Oversampling.Ratio = 1;
	if (HAL_ADC_Init(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC2_Init 2 */

	/* USER CODE END ADC2_Init 2 */

}

/**
 * @brief ADC3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC3_Init(void)
{

	/* USER CODE BEGIN ADC3_Init 0 */

	/* USER CODE END ADC3_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC3_Init 1 */

	/* USER CODE END ADC3_Init 1 */

	/** Common config
	 */
	hadc3.Instance = ADC3;
	hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc3.Init.Resolution = ADC_RESOLUTION_8B;
	hadc3.Init.DataAlign = ADC3_DATAALIGN_RIGHT;
	hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc3.Init.LowPowerAutoWait = DISABLE;
	hadc3.Init.ContinuousConvMode = DISABLE;
	hadc3.Init.NbrOfConversion = 1;
	hadc3.Init.DiscontinuousConvMode = DISABLE;
	hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc3.Init.DMAContinuousRequests = DISABLE;
	hadc3.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
	hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc3.Init.OversamplingMode = DISABLE;
	hadc3.Init.Oversampling.Ratio = ADC3_OVERSAMPLING_RATIO_2;
	if (HAL_ADC_Init(&hadc3) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC3_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC3_Init 2 */

	/* USER CODE END ADC3_Init 2 */

}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{

	/* USER CODE BEGIN CRC_Init 0 */

	/* USER CODE END CRC_Init 0 */

	/* USER CODE BEGIN CRC_Init 1 */

	/* USER CODE END CRC_Init 1 */
	hcrc.Instance = CRC;
	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CRC_Init 2 */

	/* USER CODE END CRC_Init 2 */

}

/**
 * @brief DMA2D Initialization Function
 * @param None
 * @retval None
 */
static void MX_DMA2D_Init(void)
{

	/* USER CODE BEGIN DMA2D_Init 0 */

	/* USER CODE END DMA2D_Init 0 */

	/* USER CODE BEGIN DMA2D_Init 1 */

	/* USER CODE END DMA2D_Init 1 */
	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_M2M;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
	hdma2d.Init.OutputOffset = 0;
	hdma2d.LayerCfg[1].InputOffset = 0;
	hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
	hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hdma2d.LayerCfg[1].InputAlpha = 0;
	hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
	hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
	hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DMA2D_Init 2 */

	/* USER CODE END DMA2D_Init 2 */

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
	hi2c1.Init.Timing = 0x10B0DCFB;
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
	hspi1.Init.Direction = SPI_DIRECTION_1LINE;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 0x0;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
	hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
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

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 19200-1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 10000-1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 192-1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 1000-1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

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
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SOLENOID_TEAR_1_GPIO_Port, SOLENOID_TEAR_1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOF, SOLENOID_LOCK_Pin|LAMP_Pin|ULTRASONIC_ENABLE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, SOLENOID_CLAMP_Pin|SOLENOID_TEAR_2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : PE2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : HEATER_TEMP_Pin TOUCHSCREEN_Y_Pin TOUCHSCREEN_X_Pin */
	GPIO_InitStruct.Pin = HEATER_TEMP_Pin|TOUCHSCREEN_Y_Pin|TOUCHSCREEN_X_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : PE4 PE5 PE6 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : SOLENOID_TEAR_1_Pin */
	GPIO_InitStruct.Pin = SOLENOID_TEAR_1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SOLENOID_TEAR_1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : SOLENOID_LOCK_Pin LAMP_Pin ULTRASONIC_ENABLE_Pin */
	GPIO_InitStruct.Pin = SOLENOID_LOCK_Pin|LAMP_Pin|ULTRASONIC_ENABLE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin : HEATER_Pin */
	GPIO_InitStruct.Pin = HEATER_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(HEATER_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PLATFORM_TOUCH_Pin */
	GPIO_InitStruct.Pin = PLATFORM_TOUCH_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(PLATFORM_TOUCH_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : ULTRASONIC_CURRENT_Pin ULTRASONIC_VOLTAGE_Pin */
	GPIO_InitStruct.Pin = ULTRASONIC_CURRENT_Pin|ULTRASONIC_VOLTAGE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pins : PF6 PF8 PF9 */
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin : BUTTON_START_BOND_Pin */
	GPIO_InitStruct.Pin = BUTTON_START_BOND_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(BUTTON_START_BOND_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PF10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin : PC0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PC1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PC2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : SPI1_CS_Pin */
	GPIO_InitStruct.Pin = SPI1_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PA4 PA6 PA9 PA10 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA7 PA11 PA12 */
	GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_11|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PC5 PC6 PC7 PC10
                           PC12 */
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10
			|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PE10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PE11 */
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_OCTOSPIM_P1;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PE12 */
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : PB10 PB11 PB8 */
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PG6 PG7 PG8 */
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pins : PC8 PC9 */
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA8 */
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF13_LTDC;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PD2 */
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : PD3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : PD6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : PG9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : PG10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pins : SOLENOID_CLAMP_Pin SOLENOID_TEAR_2_Pin */
	GPIO_InitStruct.Pin = SOLENOID_CLAMP_Pin|SOLENOID_TEAR_2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PB5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF3_LTDC;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(HEATER_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(HEATER_EXTI_IRQn);

	HAL_NVIC_SetPriority(PLATFORM_TOUCH_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(PLATFORM_TOUCH_EXTI_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void measure_joystick_and_send_to_slave(void)
{
	float divider;
	ADC_valX = ADC_raw[0] - 127.5;
	ADC_valY = ADC_raw[1] - 127.5;         // Read The ADC Conversion Result
	/*
	 * Fine: /250.0
	 * Coarse: /50.0
	 */
	if (fine_adjustment)
		divider = 100.0;
	else
		divider = 1.0;

	if (fabs(ADC_valX) > 40)
				speed_joystickX = ADC_valX;
			else
				speed_joystickX = 0;

			if (fabs(ADC_valY) > 40)
				speed_joystickY = ADC_valY;
			else
				speed_joystickY = 0;

			speed_joystickT = speed_joystickX;
			speed_joystickZ = speed_joystickY;

	switch (control_mode)
	{
	case 0:
		write_float_to_slave(ADDRESS_MOTOR_X, VIRTUAL_MEM_SPEED, speed_joystickX / divider);
		HAL_Delay(10);
		write_float_to_slave(ADDRESS_MOTOR_Y, VIRTUAL_MEM_SPEED, speed_joystickY / divider);
		HAL_Delay(10);
		break;
	case 1:
		write_float_to_slave(ADDRESS_MOTOR_Z, VIRTUAL_MEM_SPEED, speed_joystickZ / divider);
		HAL_Delay(10);
		break;
	case 2:
		write_float_to_slave(ADDRESS_MOTOR_T, VIRTUAL_MEM_SPEED, speed_joystickT / divider);
		HAL_Delay(10);
		break;
	}
}

// Enter into jog mode (XY movement with joystick)
void enter_jog_mode(void)
{
//	write_uint32_to_slave(ADDRESS_MOTOR_X, VIRTUAL_MEM_MODE, SPD_MODE);
//	write_uint32_to_slave(ADDRESS_MOTOR_Y, VIRTUAL_MEM_MODE, SPD_MODE);
	while (HAL_GPIO_ReadPin(BUTTON_START_BOND_GPIO_Port, BUTTON_START_BOND_Pin) == GPIO_PIN_SET)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1000);
		ADC_raw[0] = HAL_ADC_GetValue(&hadc1);

		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, 1000);
		ADC_raw[1] = HAL_ADC_GetValue(&hadc3);

		//Enable XY jog mode
		measure_joystick_and_send_to_slave();
		//HAL_Delay(100)

		//measure_joystick_and_send_to_slave();
		//HAL_Delay(100);
	}
	//write_uint32_to_slave(ADDRESS_MOTOR_X, VIRTUAL_MEM_MODE, POS_MODE);
	//write_uint32_to_slave(ADDRESS_MOTOR_Y, VIRTUAL_MEM_MODE, POS_MODE);
}

/** @todo: move this stuff some other place */
int32_t setpoint = 322042;	// 50 °C
const int32_t Kp = 90, Ki = 0, Sample_time = 1;	// PI parameters

#define ADC_MIN 288      // Minimum ADC value allowed
#define ADC_MAX 3456     // Maximum ADC value allowed
#define DECIMATION 5     // Bit shift for dividing by 32 (2^5)
#define LOOKUP_SIZE 100  // Size of lookup table
#define PLMAX 2048		// Proportional Limit Max
#define OLMAX 1000		// Output PWM Limit Max
#define DELAYTAPS 22
#define LOOKUP_SIZE 100  // Size of lookup table

/* to obtain degrees in °C: value/2^10-273 */
int32_t Tlook[LOOKUP_SIZE] = {	// start 512; step 32; stop 3712
		293603, 296365, 298924, 301313, 303559, 305683, 307701, 309627, 311471, 313243,
		314951, 316601, 318200, 319752, 321261, 322732, 324167, 325570, 326944, 328291,
		329612, 330911, 332189, 333448, 334689, 335913 ,337122, 338317, 339500, 340671,
		341831, 342982, 344123, 345257, 346383, 347503 ,348617, 349726, 350831, 351932,
		353030, 354125, 355219, 356312, 357403 ,358495 ,359587, 360681, 361775 ,362872,
		363972, 365075, 366182, 367293, 368409, 369531 ,370659, 371793, 372935, 374085,
		375244, 376412, 377591, 378780, 379980, 381193, 382419, 383659, 384914, 386185,
		387473, 388778, 390103, 391448, 392815, 394204 ,395618, 397057, 398524, 400020,
		401547, 403107, 404703, 406336, 408009, 409726, 411489 ,413301, 415167, 417090,
		419076, 421129, 423254, 425459, 427751, 430136, 432626, 435230, 437961, 440832};

/**** Smith Predictor ****/

const int32_t au = 65536, ad = 65477, bu = 209, bd = 209;	// Model equivalent Filter coefficients

int32_t in_model_prev = 0, out_model_prev = 0, in_model = 0, out_model = 0, output2;
int32_t  out_delayed;
int32_t delay_vect[DELAYTAPS] = {0};
uint32_t index1=0, index2=1;
int32_t feedback_pred =0;


float read_temp_from_ADC(void)
{
	uint32_t ADC_value;

	ADC_value = HAL_ADC_GetValue(&hadc2);

	if (ADC_value < ADC_MIN)
	{
		ADC_value = ADC_MIN;
	}
	else if (ADC_value > ADC_MAX)
	{
		ADC_value = ADC_MAX;
	}

	//Read_temp = Tlook[ADC_value - 195];  //209

	// Interpolating...

	int16_t adc_index = (ADC_value >> DECIMATION) - (ADC_MIN >> DECIMATION);

	int32_t delta_T = Tlook[adc_index + 1] - Tlook[adc_index];

	int16_t delta_ADC = ADC_value - ((adc_index + (ADC_MIN >> DECIMATION)) << DECIMATION);

	return ((delta_T * delta_ADC) + (Tlook[adc_index] << DECIMATION)) >> DECIMATION;
}

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
	int32_t Read_temp, PI_error, PI_output, integral = 0, proportional, PI_prev_error = 0, limMaxInt = 0, limMinInt = 0;

	if(htim == &htim2) {
		/* Read ADC */
		HAL_ADC_Start(&hadc2);
		HAL_ADC_PollForConversion(&hadc2, 1000);
		Read_temp = read_temp_from_ADC();
		HAL_ADC_Stop(&hadc2);

		measured_temperature_gauge = ((float)(Read_temp>>10)) - 273.5;
		temperature_testpoint = Read_temp;

		/* PI Controller */
		//error = setpoint - feedback_pred;
		PI_error = setpoint - Read_temp;

		proportional = Kp * PI_error;

		// Dynamic Anti-wind-up limits calculation
		if (PLMAX > proportional && proportional >= 0) {
			limMaxInt = PLMAX - proportional;
		}
		else {
			limMaxInt = 0;
		}

		integral = integral + Ki * Sample_time * (PI_prev_error + PI_error);
		PI_prev_error = PI_error;

		// Anti-wind-up integrator limits clamping
		if (integral > limMaxInt) {
			integral = limMaxInt;
		}
		else if (integral < limMinInt) {
			integral = limMinInt;
		}

		// output to PWM value conditioning
		PI_output = ((integral + proportional) >> 10) * 50;

		if (PI_output > OLMAX) {
			PI_output = 1000;
		}
		else if (PI_output < 0) {
			PI_output = 0;
		}
		/* End PI Controller */

		TIM3->CCR3 = PI_output;

		// Output diventa 2^10
		output2 = PI_output * 20;

		// 2^20 -> 2^10
		in_model = (((output2 * output2) >> 10) * 82) >> 10;

		//L'uscita è 2^10
		out_model =  (out_model_prev * ad + bu * in_model + bd * in_model_prev) >> 16;

		in_model_prev = in_model;
		out_model_prev = out_model;

		// Prendo primo blocco
		out_delayed = delay_vect[index1];

		delay_vect[index1] = out_model;

		index1++;

		if (index1 > 21)
			index1 = 0;


		feedback_pred = Read_temp - out_delayed + out_model;

	}
}


void write_float_to_slave(uint8_t slave_address, uint8_t virtual_memory_address, float value)
{
	ui32_to_f conv;
	conv.f = value;
	prepare_WRITE_packet(tx_buffer, virtual_memory_address, conv.ui32);
	HAL_I2C_Master_Transmit(&hi2c1, slave_address, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
}

void write_uint32_to_slave(uint8_t slave_address, uint8_t virtual_memory_address, uint32_t value)
{
	prepare_WRITE_packet(tx_buffer, virtual_memory_address, value);
	HAL_I2C_Master_Transmit(&hi2c1, slave_address, tx_buffer, MAX_COMM_BUFFER_SIZE, MAX_TRANSMISSION_TIME);
}

void perform_bond(uint32_t bond_time)
{
	//Perform the bond

	//Enable the ultrasonic driver
	HAL_GPIO_WritePin(ULTRASONIC_ENABLE_GPIO_Port, ULTRASONIC_ENABLE_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	write_bond_power_to_trimmer(bond_power);
	HAL_Delay(bond_time);
	write_bond_power_to_trimmer(0);
	HAL_Delay(100);
	//Disable the ultrasonic driver
	HAL_GPIO_WritePin(ULTRASONIC_ENABLE_GPIO_Port, ULTRASONIC_ENABLE_Pin, GPIO_PIN_SET);
}

void wait_for_button_release(void)
{
	while(HAL_GPIO_ReadPin(BUTTON_START_BOND_GPIO_Port, BUTTON_START_BOND_Pin) == GPIO_PIN_RESET)
		HAL_Delay(100);
}

void wait_for_button_press(void)
{
	// Wait for button press
	while(HAL_GPIO_ReadPin(BUTTON_START_BOND_GPIO_Port, BUTTON_START_BOND_Pin) == GPIO_PIN_SET)
		HAL_Delay(100);
	// Wait for the user to release the button
	wait_for_button_release();
}

void write_bond_power_to_trimmer(uint8_t bond_power)
{
	uint8_t payload[2] = {0x11, bond_power};
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, payload, 2, 100);
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

	return;
}

/**
 * @brief Handles the GPIO callback
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == PLATFORM_TOUCH_Pin)
	{
		head_touched = true;
	}
}

int execute_write_command(uint8_t write_address, uint32_t data)
{
	int ret_val = COMM_ERR;
	ui32_to_f conv;
	conv.ui32 = data;

	switch (write_address)
	{
	case VIRTUAL_MEM_POSITION_MEAS:
		read_position = conv.f;
		data_ready = true;
		ret_val = COMM_OK;
		break;

	default:
		ret_val = COMM_ERR;
		break;
	}

	return ret_val;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c1)
{
	//Perform different actions based on the received command
	switch (rx_buffer[0])
	{
	case COMMAND_WRITE:
		read_data_from_WRITE_command(rx_buffer, &received_address, &received_data);
		execute_write_command(received_address, received_data);
		break;
	}
	return;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

	if(hadc == &hadc1)
		ADC_ready = 1;
}


/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct = {0};

	/* Disables the MPU */
	HAL_MPU_Disable();

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x0;
	MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
	MPU_InitStruct.SubRegionDisable = 0x87;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
