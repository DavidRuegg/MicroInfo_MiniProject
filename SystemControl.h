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
#define ONE_CELL 				923		// steps for one cell (experimental)
#define DEGREE_2_STEPS			3.6 	// experimental
#define MOVE_FORWARD			0		// continue forward, no turn
#define LEFT_TURN				-324	// steps for 90 degree turn left (experimental)
#define RIGHT_TURN				324 	// steps for 90 degree turn right (experimental)
#define BACKWARD_TURN			648 	// steps for 180 degree turn (experimental)

#define POSITION_NOT_REACHED	0
#define POSITION_REACHED       	1
#define LWF_ALGORITHM			0
#define PLEDGE_ALGORITHM		1



void turn(int16_t angle);
void go_next_cell(int16_t distance);
void move(uint8_t algorithm, uint8_t actual_cell);
void control_motor_start(void);

#endif /* SYSTEMCONTROL_H_ */
