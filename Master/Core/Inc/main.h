/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HEATER_TEMP_Pin GPIO_PIN_3
#define HEATER_TEMP_GPIO_Port GPIOE
#define SOLENOID_TEAR_1_Pin GPIO_PIN_13
#define SOLENOID_TEAR_1_GPIO_Port GPIOC
#define SOLENOID_LOCK_Pin GPIO_PIN_0
#define SOLENOID_LOCK_GPIO_Port GPIOF
#define LAMP_Pin GPIO_PIN_1
#define LAMP_GPIO_Port GPIOF
#define HEATER_Pin GPIO_PIN_2
#define HEATER_GPIO_Port GPIOF
#define HEATER_EXTI_IRQn EXTI2_IRQn
#define PLATFORM_TOUCH_Pin GPIO_PIN_3
#define PLATFORM_TOUCH_GPIO_Port GPIOF
#define PLATFORM_TOUCH_EXTI_IRQn EXTI3_IRQn
#define ULTRASONIC_CURRENT_Pin GPIO_PIN_4
#define ULTRASONIC_CURRENT_GPIO_Port GPIOF
#define ULTRASONIC_VOLTAGE_Pin GPIO_PIN_5
#define ULTRASONIC_VOLTAGE_GPIO_Port GPIOF
#define BUTTON_START_BOND_Pin GPIO_PIN_7
#define BUTTON_START_BOND_GPIO_Port GPIOF
#define SPI1_CS_Pin GPIO_PIN_3
#define SPI1_CS_GPIO_Port GPIOA
#define ULTRASONIC_ENABLE_Pin GPIO_PIN_12
#define ULTRASONIC_ENABLE_GPIO_Port GPIOF
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SOLENOID_CLAMP_Pin GPIO_PIN_3
#define SOLENOID_CLAMP_GPIO_Port GPIOB
#define SOLENOID_TEAR_2_Pin GPIO_PIN_4
#define SOLENOID_TEAR_2_GPIO_Port GPIOB
#define TOUCHSCREEN_Y_Pin GPIO_PIN_0
#define TOUCHSCREEN_Y_GPIO_Port GPIOE
#define TOUCHSCREEN_X_Pin GPIO_PIN_1
#define TOUCHSCREEN_X_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
