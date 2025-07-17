/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file  main.h
 * @brief Header for main.c file.
 *        This file contains the common defines of the application.
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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdbool.h>
#include "serial_communication.h"
#include "controller.h"
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
#define FAULTB_Pin GPIO_PIN_2
#define FAULTB_GPIO_Port GPIOC
#define FAULTA_Pin GPIO_PIN_3
#define FAULTA_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define ALIVE_LED_Pin GPIO_PIN_10
#define ALIVE_LED_GPIO_Port GPIOC
#define COM_LED_Pin GPIO_PIN_11
#define COM_LED_GPIO_Port GPIOC
#define FAULT_LED_Pin GPIO_PIN_12
#define FAULT_LED_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LIMA_Pin GPIO_PIN_5
#define LIMA_GPIO_Port GPIOB
#define LIMA_EXTI_IRQn EXTI9_5_IRQn
#define LIMB_Pin GPIO_PIN_6
#define LIMB_GPIO_Port GPIOB
#define LIMB_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */
/**
 * Sets different parameters based on the axis being controlled
 * 0: X
 * 1: Y
 * 2: Z
 * 3: Theta
 */
#define MOTOR_AXIS 2

#if MOTOR_AXIS == 0
#define SLAVE_ADDRESS ADDRESS_MOTOR_X
#endif
#if MOTOR_AXIS == 1
#define SLAVE_ADDRESS ADDRESS_MOTOR_Y
#endif
#if MOTOR_AXIS == 2
#define SLAVE_ADDRESS ADDRESS_MOTOR_Z
#endif
#if MOTOR_AXIS == 3
#define SLAVE_ADDRESS ADDRESS_MOTOR_T
#endif

/** Joystick position control mode */
#define POS_MODE 0

/** Debug only */
#define SPD_MODE 3

/** Joystick speed control mode */
#define VEL_MODE 1

#define FULL_MODE 2

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
