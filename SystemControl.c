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

static int32_t position_to_reach = 0;		// in [step]
static uint8_t position_left_reached = 1;
static uint8_t position_right_reached = 1;
static int16_t speed_left = 0;
static int16_t speed_right = 0;

/**
 * @brief	Set the position to reach for each motor and the speed
 * 			for the e-puck to do a turn.
 *
 * @param angle		Value in steps corresponding to the number needed to do the turn,
 * 					positive value to turn right, negative to turn left.
 */
void turn(int16_t angle){
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	position_to_reach = abs(angle);

	position_left_reached = POSITION_NOT_REACHED;
	position_right_reached = POSITION_NOT_REACHED;

	if(angle > 0){
		// turn right
		speed_left = NOMINAL_SPEED;
		speed_right = -NOMINAL_SPEED;
	}else{
		// turn left
		speed_left = -NOMINAL_SPEED;
		speed_right = NOMINAL_SPEED;
	}
}

/**
 * @brief	Set the position to reach for each motor and the speed
 * 			for the e-puck to move forward for a certain distance.
 *
 * @param distance		Value in steps corresponding to the number needed to move the
 * 						expected length.
 */
void go_next_cell(int16_t distance){
	left_motor_set_pos(0);
	right_motor_set_pos(0);
	position_to_reach = distance;
	position_left_reached = POSITION_NOT_REACHED;
	position_right_reached = POSITION_NOT_REACHED;

	speed_left = NOMINAL_SPEED;
	speed_right = NOMINAL_SPEED;

}


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

/**
 * @brief	Set the position to reach for each motor and the speed
 * 			for the e-puck to turn and then move forward/only move forward
 * 			for a fixed distance of one cell of the maze.
 *
 * @param direction		Value in steps corresponding to the number needed to do the turn,
 * 						positive value to turn right, negative to turn left, if 0 only moves
 * 						forward.
 */
void move(uint16_t direction){

	chBSemWait(&motor_ready_sem);		// necessary so it don't do weird things... -_-'

	if(!(direction == MOVE_FORWARD)){
		turn(direction);
		chBSemWait(&motor_ready_sem);
	}

	go_next_cell(ONE_CELL);
}

void control_motor_start(void){
	chThdCreateStatic(waControlMotor, sizeof(waControlMotor), NORMALPRIO+1, ControlMotor, NULL);
}
