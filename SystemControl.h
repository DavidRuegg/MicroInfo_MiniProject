/*
 * SystemControl.h
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#ifndef SYSTEMCONTROL_H_
#define SYSTEMCONTROL_H_

#include <stdint.h>

#define NOMINAL_SPEED			500
#define STOP_SPEED				0
#define NSTEP_ONE_REVOLUTION	1000
#define MOVE_FORWARD			0		// continue forward, no turn
#define MM_2_STEPS				7.65
#define ONE_CELL 				880 	// steps for one cell (experimental)
#define DEGREE_2_STEPS			3.60
#define LEFT_TURN				-324	// steps for 90 degree turn left (experimental)
#define RIGHT_TURN				324 	// steps for 90 degree turn right (experimental)
#define BACKWARD_TURN			648 	// steps for 180 degree turn (experimental)

#define POSITION_NOT_REACHED	0
#define POSITION_REACHED       	1

/**
 * @brief	Set the position to reach for each motor and the speed
 * 			for the e-puck to do a turn.
 *
 * @param angle		Value in steps corresponding to the number needed to do the turn,
 * 					positive value to turn right, negative to turn left.
 */
void turn(int16_t angle);

/**
 * @brief	Set the position to reach for each motor and the speed
 * 			for the e-puck to move forward for a certain distance.
 *
 * @param distance		Value in steps corresponding to the number needed to move the
 * 						expected length.
 */
void go_next_cell(int16_t distance);

/**
 * @brief	Set the position to reach for each motor and the speed
 * 			for the e-puck to turn and then move forward/only move forward
 * 			for a fixed distance of one cell of the maze.
 *
 * @param direction		Value in steps corresponding to the number needed to do the turn,
 * 						positive value to turn right, negative to turn left, if 0 only moves
 * 						forward.
 */
void move(uint16_t direction);

void control_motor_start(void);

#endif /* SYSTEMCONTROL_H_ */
