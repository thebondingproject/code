/**
 * @file controller.h
 * @brief Header file for controller.c
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <main.h>

/**********************************************************************************************
 *  Defines
 **********************************************************************************************/
/** @todo Are we sure these are defines? */
/** Number of impulses per encoder rotation */
// 0.02 Z, 0.03 theta  0.05 XY
#define PPR_ENCODER 1024.0f

#if MOTOR_AXIS == 0 || MOTOR_AXIS == 1
	/** Sets the bandwidth of the speed controller  */
	#define ALPHA 0.05f //0.38587 bandwidth α = T/(T+tau) (T=sample time, tau = filter pole) T=0.000639, tau=0.001017
	#define K_SPRING 0;
#endif
#if MOTOR_AXIS == 2
	/** Sets the bandwidth of the speed controller  */
	#define ALPHA 0.02f //0.38587 bandwidth α = T/(T+tau) (T=sample time, tau = filter pole) T=0.000639, tau=0.001017
	#define K_SPRING 0;
#endif
#if MOTOR_AXIS == 3
	/** Sets the bandwidth of the speed controller  */
	#define ALPHA 0.03f //0.38587 bandwidth α = T/(T+tau) (T=sample time, tau = filter pole) T=0.000639, tau=0.001017
	#define K_SPRING 0;
#endif

/**********************************************************************************************
 *  Structures
 **********************************************************************************************/

/**
 * Structure containing loop parameters to be used in the PI_controller function
 * @todo Rivedere docs
 */
struct loop_parameters
{
	float P;        /**< Proportional gain */
	float I;        /**< Integral gain */
	float LimMax;   /**< Maximum output value (upper saturation) */
	float LimMin;   /**< Minimum output value (upper saturation) */
	float integral; /**< Memory (integrator) */
	float T;        /**< Time step */
};

/**********************************************************************************************
 *  Function primitives
 **********************************************************************************************/
float PI_controller(float setpoint, float feedback, struct loop_parameters *parameter);
void controller_variables_init(void);
float motion_profile(float max_acceleration, float max_velocity, float target_position, float position_at_call, float current_time, float time_at_call);

/**********************************************************************************************
 *  Global variables externals
 **********************************************************************************************/
extern struct loop_parameters loop_parameters_current;
extern struct loop_parameters loop_parameters_speed;
extern struct loop_parameters loop_parameters_position;
extern float last_position_max;
extern float last_position_min;
extern bool motor_halt_max;
extern bool motor_halt_min;
extern uint32_t interrupt_counter;
extern float measured_position;
extern float old_position;
extern float setpoint_position;
extern float measured_current;
extern float output_current;
extern float setpoint_current;
extern float filtered_speed;
extern float setpoint_speed;
extern float measured_speed;
extern float target_position;
extern uint32_t time_at_call;
extern float position_at_call;
extern float spring_compensation_current;
extern float R;
extern float L;

#endif /* INC_CONTROLLER_H_ */
