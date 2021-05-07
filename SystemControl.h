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
#define MM_2_STEPS				7.68
#define ONE_CELL 				880 //875		// steps for one cell (experimental)
#define DEGREE_2_STEPS			3.422
//#define LEFT_TURN				-301	// steps for 90 degree turn left (experimental)
#define LEFT_TURN				-324	// steps for 90 degree turn left (experimental)
//#define RIGHT_TURN				301 	// steps for 90 degree turn right (experimental)
#define RIGHT_TURN				324 	// steps for 90 degree turn right (experimental)
#define BACKWARD_TURN			648 	// steps for 180 degree turn (experimental)

#define POSITION_NOT_REACHED	0
#define POSITION_REACHED       	1




void turn(int32_t angle);
void go_next_cell(int16_t distance);
void control_motor_start(void);

#endif /* SYSTEMCONTROL_H_ */
