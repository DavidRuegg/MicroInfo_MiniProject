/*
 * SystemControl.c
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#include <stdlib.h>

#include <motors.h>

#include <SystemControl.h>
#include <DataAcquisition.h>
#include <DataProcess.h>

/*** GLOBAL VARIABLES ***/
BSEMAPHORE_DECL(motor_ready_sem, FALSE);

/*** STATIC VARIABLES ***/

static int16_t position_to_reach = 0;		// in [steps]
static uint8_t position_left_reached = 1;	// 1 == reached, 0 == not reached
static uint8_t position_right_reached = 1;	// 1 == reached, 0 == not reached
static int16_t speed_left = 0;				// in [step/s]
static int16_t speed_right = 0;				// in [step/s]

/*** INTERNAL FUNCTIONCS ***/

/**
 * @brief	Thread which controls if the positions has been reached by the motors.
 * 			Signals semaphore motor_ready_sem when positions are reached.
 * 			Sets speed consequently to static variables speed_left and speed_right.
 */
static THD_WORKING_AREA(waControlMotor, 256);
static THD_FUNCTION(ControlMotor, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	while(1){
		time = chVTGetSystemTime();

		if(!position_left_reached){
			if(abs(left_motor_get_pos()) >= position_to_reach){
				position_left_reached = POSITION_REACHED;
				speed_left = STOP_SPEED;
			}
			left_motor_set_speed(speed_left);
		}

		if(!position_right_reached){
			if(abs(right_motor_get_pos()) >= position_to_reach){
				position_right_reached = POSITION_REACHED;
				speed_right = STOP_SPEED;
			}
			right_motor_set_speed(speed_right);
		}

		if(position_left_reached && position_right_reached){
			chBSemSignal(&motor_ready_sem);
		}

		//20Hz
		chThdSleepUntilWindowed(time, time + MS2ST(50));

	}
}

/*** END INTERNAL FUNCTIONCS ***/

/*** PUBLIC FUNCTIONCS ***/

void control_motor_start(void){
	chThdCreateStatic(waControlMotor, sizeof(waControlMotor), NORMALPRIO+1, ControlMotor, NULL);
}

void turn(int16_t angle){
	// Resets left and right motors position
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	// Sets position to reach
	position_to_reach = abs(angle);

	// Resets position reached
	position_left_reached = POSITION_NOT_REACHED;
	position_right_reached = POSITION_NOT_REACHED;

	if(angle > 0){						// turn right
		speed_left = NOMINAL_SPEED;
		speed_right = -NOMINAL_SPEED;
	}else{								// turn left
		speed_left = -NOMINAL_SPEED;
		speed_right = NOMINAL_SPEED;
	}
}

void go_next_cell(int16_t distance){
	// Resets left and right motors position
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	// Sets position to reach
	position_to_reach = abs(distance);

	// Resets position reached
	position_left_reached = POSITION_NOT_REACHED;
	position_right_reached = POSITION_NOT_REACHED;

	if(distance > 0){					// go forward
		speed_left = NOMINAL_SPEED;
		speed_right = NOMINAL_SPEED;
	}else{								// go backward
		speed_left = -NOMINAL_SPEED;
		speed_right = -NOMINAL_SPEED;
	}
}

void move(int16_t direction){
	// Waits that the motors have reached their positions before a new command
	chBSemWait(&motor_ready_sem);

	// turn if necessary
	if(!(direction == MOVE_FORWARD)){
		turn(direction);
		chBSemWait(&motor_ready_sem);
	}

	// move to next cell
	go_next_cell(ONE_CELL);
}
