/**
 * @file controller.c
 * @brief Functions and variables for motor control
 */

#include "controller.h"
#include "main.h"

/**********************************************************************************************
 *  Global variables
 **********************************************************************************************/

/** Loop parameters for current */
struct loop_parameters loop_parameters_current;
/** Loop parameters for speed */
struct loop_parameters loop_parameters_speed;
/** Loop parameters for position*/
struct loop_parameters loop_parameters_position;
/** Last position when max limit switch is activated*/
float last_position_max;
/** Last position when min limit switch is activated*/
float last_position_min;
/** Motor halt flag when max limit switch is activated*/
bool motor_halt_max;
/** Motor halt flag when min limit switch is activated*/
bool motor_halt_min;
/** Counts how many times the ADC end-of-conversion callback is called.
 * It's used to reduce the update frequency of the position and control loop
 * as they need fewer updates compared to the current control loop.
 * (the current loop is updated everytime; the position and speed get calculated
 * every N times)
 */
uint32_t interrupt_counter;
/** Current motor position */
float measured_position;
/** @todo Do we need this here? */
float old_position;
/** Desired final motor position */
float target_position;
/** Intermediate values of motor position during movement. It's set by the motion_profile function */
float setpoint_position;
/** Current motor speed */
float measured_speed;
/** Desired speed value calculated by the PI position control loop */
float setpoint_speed;
/** @todo Needs docs */
float filtered_speed;
/** Current motor current */
float measured_current;
/** Desired current value calculated by the PI speed control loop */
float setpoint_current;
/** @todo Do we need this here? */
float output_current;
/** Current time when movement is started (needed by the motion_profile function) */
uint32_t time_at_call;
/** Current position when movement is started (needed by the motion_profile function) */
float position_at_call;

float spring_compensation_current;

float R;
float L;

/**********************************************************************************************
 *  Function declarations
 **********************************************************************************************/

/**
 * @brief Implements a PI controller given loop parameters.
 *
 * @param setpoint  desired final value
 * @param feedback  current value
 * @param parameter loop parameters
 * @return current value to actuate
 */
float PI_controller(float setpoint, float feedback, struct loop_parameters *parameter)
{
	float error = setpoint - feedback;

	float proportional = error * parameter->P;
	parameter->integral = parameter->integral + proportional * parameter->I * parameter->T;

	// Dynamic Anti-wind-up limits calculation
	float limMaxInt, limMinInt;

	if (parameter->LimMax > proportional)
		limMaxInt = parameter->LimMax - proportional;
	else
		limMaxInt = 0;

	if (parameter->LimMin < proportional)
		limMinInt = parameter->LimMin - proportional;
	else
		limMinInt = 0;

	// Clamp integral
	if (parameter->integral > limMaxInt)
		parameter->integral = limMaxInt;
	else if (parameter->integral < limMinInt)
		parameter->integral = limMinInt;

	float output = proportional + parameter->integral;

	// Output saturation
	if (output > parameter->LimMax)
		output = parameter->LimMax;
	else if (output < parameter->LimMin)
		output = parameter->LimMin;

	return output;
}

/**
 * @brief Initializes the PI controller parameters.
 */
void controller_variables_init(void)
{

#if MOTOR_AXIS == 0 || MOTOR_AXIS == 1  //X and Y motor
	// Loop paramaters
	loop_parameters_current = (struct loop_parameters
	)
					{ .P = 7.1168f, .I = 4926.4f, .LimMax = 12, .LimMin = 0, .T = 0.0000639f };
	loop_parameters_speed = (struct loop_parameters
	)
					{ .P = 0.097791,   .I = 12.823f, .LimMax = 1, .LimMin = -1, .T = 0.00031969f }; //0.11409  0.097791 17.453  12.823
	loop_parameters_position = (struct loop_parameters
	)
					{ .P = 50.0f, .I = 0.1f, .LimMax = 30, .LimMin = -30, .T = 0.00063939f };

	spring_compensation_current = 0;
	R = 5.58f;
	L = 0.00113f;
#endif

#if MOTOR_AXIS == 2 //Z motor (23062025 DONE)
	// Loop paramaters
	loop_parameters_current = (struct loop_parameters
	)
					{ .P = 37.7f, .I = 1000.0f, .LimMax = 12, .LimMin = 0, .T = 0.0000639f };
	loop_parameters_speed = (struct loop_parameters
	)
					{ .P = 0.23909f, .I = 50.265f, .LimMax = 1.8, .LimMin = -1.8, .T = 0.00031969f };
	loop_parameters_position = (struct loop_parameters
	)
					{ .P = 100.0f, .I = 0.1f, .LimMax = 3, .LimMin = -3, .T = 0.00063939f };

	spring_compensation_current = -0.6; // 0.6
	R = 6;
	L = 0.006f;
#endif

#if MOTOR_AXIS == 3 //Theta motor
	// Loop paramaters
	loop_parameters_current = (struct loop_parameters
	)
					{ .P = 40.212f, .I = 812.5f, .LimMax = 12, .LimMin = 0, .T = 0.0000639f };
	loop_parameters_speed = (struct loop_parameters
	)
					{ .P = 0.50076f, .I = 17.453f, .LimMax = 1.8, .LimMin = -1.8, .T = 0.00031969f };
	loop_parameters_position = (struct loop_parameters
	)
					{ .P = 65.0f, .I = 0.01f, .LimMax = 10, .LimMin = -10, .T = 0.00063939f };

	spring_compensation_current = 0; //-0.45f
	R = 5.2;
	L = 0.0064f;
#endif
	motor_halt_min = false;
	interrupt_counter = 0;
	old_position = 0.0f;
	setpoint_position = 0.0f;
}

/**
 * @brief Calculate the position based on the given motion profile times, maximum acceleration, velocity, and current time.
 *
 * @param max_acceleration maximum allowed acceleration [rad/s^2]
 * @param max_velocity     maximum allowed speed [rad/s]
 * @param target_position  desired final motor position
 * @param position_at_call initial motor position when motion_profile function is called (this value must remain CONSTANT until the trajectory is complete)
 * @param current_time     current time [ms]
 * @param time_at_call     time at which motion_profile function is called, in [ms] (this value must remain CONSTANT until the trajectory is complete)
 *
 * @return calculated motor position
 *
 * @note This function was derived from this article: https://www.ctrlaltftc.com/advanced/motion-profiling
 */
float motion_profile(float max_acceleration, float max_velocity, float target_position, float position_at_call, float current_time, float time_at_call)
{
	float distance = target_position - position_at_call;

	/*
	 * Don't perform any calculations if the motor movement is 0.
	 * Otherwise the function returns NaN and everything breaks.
	 */

	if(distance == 0)
		return position_at_call;

	float elapsed_time = (current_time - time_at_call)/1000.0;

	float sign;
	// Gets the sign of the distance
	if (distance >= 0)
		sign = 1;
	else
		sign = -1;

	/*
	 * We perform all calculations using positive distance values
	 * in order not to fuck up the calculations.
	 *
	 * If the distance at call is negative, the function will
	 * return a negative value (see the "sign*" at the function returns).
	 */
	float distance_abs = sign * distance;

	// Calculate the time it takes to accelerate to max velocity
	float acceleration_dt = max_velocity / max_acceleration;

	// If we can't accelerate to max velocity in the given distance, we'll accelerate as much as possible
	float halfway_distance = distance_abs / 2;
	float acceleration_distance = 0.5 * max_acceleration * pow(acceleration_dt, 2);

	if (acceleration_distance > halfway_distance)
		acceleration_dt = sqrt(halfway_distance / (0.5 * max_acceleration));

	acceleration_distance = 0.5 * max_acceleration * pow(acceleration_dt, 2);

	// recalculate max velocity based on the time we have to accelerate and decelerate
	max_velocity = max_acceleration * acceleration_dt;

	// we decelerate at the same rate as we accelerate
	float deceleration_dt = acceleration_dt;

	// calculate the time that we're at max velocity
	float cruise_distance = distance_abs - 2 * acceleration_distance;
	float cruise_dt = cruise_distance / max_velocity;
	float deceleration_time = acceleration_dt + cruise_dt;

	// check if we're still in the motion profile
	float entire_dt = acceleration_dt + cruise_dt + deceleration_dt;

	if (elapsed_time > entire_dt)
		return position_at_call + sign * distance_abs;

	// if we're accelerating
	if (elapsed_time < acceleration_dt)
	{
		// use the kinematic equation for acceleration
		return position_at_call + sign * 0.5 * max_acceleration * pow(elapsed_time, 2);
	}

	// if we're cruising
	else if (elapsed_time < deceleration_time)
	{
		acceleration_distance = 0.5 * max_acceleration * pow(acceleration_dt, 2);
		float cruise_current_dt = elapsed_time - acceleration_dt;

		// use the kinematic equation for constant velocity
		return position_at_call + sign * (acceleration_distance + max_velocity * cruise_current_dt);
	}

	// if we're decelerating
	else
	{
		acceleration_distance = 0.5 * max_acceleration * pow(acceleration_dt, 2);
		cruise_distance = max_velocity * cruise_dt;
		deceleration_time = elapsed_time - deceleration_time;

		// use the kinematic equations to calculate the instantaneous desired position
		return position_at_call + sign
				* (acceleration_distance + cruise_distance + max_velocity * deceleration_time
						- 0.5 * max_acceleration * pow(deceleration_time, 2));
	}
}
