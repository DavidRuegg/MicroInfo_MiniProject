/*
 * SystemControl.h
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#ifndef SYSTEMCONTROL_H_
#define SYSTEMCONTROL_H_

#include <stdint.h>

#define NOMINAL_SPEED			500		// in [step/s]
#define CORRECTION_SPEED		150		// in [step/s]
#define STOP_SPEED				0		// in [step/s]
#define NSTEP_ONE_REVOLUTION	1000	// steps for a complete revolution
#define MOVE_FORWARD			0		// continue forward, no turn
#define MM_2_STEP				7.65	// converts millimeters to steps (experimental)
#define ONE_CELL 				880 	// steps for one cell (experimental)
#define DEGREE_2_STEP			3.60	// converts degrees to steps (experimental)
#define LEFT_TURN				-324	// steps for 90 degree turn left (experimental)
#define RIGHT_TURN				324 	// steps for 90 degree turn right (experimental)
#define BACKWARD_TURN			648 	// steps for 180 degree turn (experimental)

#define POSITION_NOT_REACHED	0
#define POSITION_REACHED       	1


/**
 * @brief	Starts thread to control if the position has been reached with
 * 			NORMALPRIO+1
 */
void control_motor_start(void);

/**
 * @brief	Increases or decreases the nominal speed.
 *
 * @param correction	Value in steps per seconds to increase/decrease the nominal speed
 * 					 	 Positive value to increase.
 * 					 	 Negative value to decrease.
 */
void correction_nominal_speed(int16_t correction);

/**
 * @brief	Sets the position to reach for each motor at nominal speed
 * 			 for the e-puck to do a turn.
 *
 * @param angle			Value in steps corresponding to the number needed to do the turn.
 * 					 	 Positive value to turn right.
 * 					 	 Negative value to turn left.
 */
void turn(int16_t angle);

/**
 * @brief	Sets the position to reach for each motor at nominal speed
 * 			 for the e-puck to move of a certain distance.
 *
 * @param distance		Value in steps corresponding to the number needed to move the expected length.
 * 						 Positive value to go forward.
 * 						 Negative value to go backward.
 */
void go_next_cell(int16_t distance);

/**
 * @brief	Sets the position to reach for each motor at nominal speed,
 * 			 for the e-puck to turn and then move forward or only move forward
 * 			 for a fixed distance of one cell of the maze.
 *
 * @param direction		Value in steps corresponding to the number needed to do the turn,
 * 						 Positive value to turn right then move forward.
 * 						 Negative to turn left then move forward.
 * 						 0 if only move forward.
 */
void move(int16_t direction);

#endif /* SYSTEMCONTROL_H_ */
