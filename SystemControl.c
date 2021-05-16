/**
 * @file	SystemControl.c
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	16.05.2021
 *
 * @brief	Thread to control position in steps of an e-puck robot.
 * 			Functions to drive the e-puck robot (turn, move forward).
 * 			Global semaphore to advertise that the motors are ready for a new command.
 */

#include <motors.h>

#include <main.h>
#include <SystemControl.h>


/*** GLOBAL VARIABLES ***/
BSEMAPHORE_DECL(MotorReady_sem, FALSE);
thd_metadata_t ControlMotor_MetaData = {.Sleep = 0, .ThdReference = NULL};


/*** STATIC VARIABLES ***/
static uint8_t PositionLeft_Reached 	= 1;	// 1 == reached, 0 == not reached
static uint8_t PositionRight_Reached 	= 1;	// 1 == reached, 0 == not reached
static int16_t Position2Reach 			= 0;	// in [steps]
static int16_t NominalSpeed = NOMINAL_SPEED;	// in [step/s]
static int16_t SpeedLeft 				= 0;	// in [step/s]
static int16_t SpeedRight 				= 0;	// in [step/s]

/*** INTERNAL FUNCTIONCS ***/

/**
 * @brief	Thread which controls if the positions has been reached by the motors.
 * 			Signals semaphore MotorReady_sem when positions are reached.
 * 			Sets speed consequently to static variables SpeedLeft and SpeedRight.
 */
static THD_WORKING_AREA(waControlMotor, 128);
static THD_FUNCTION(ControlMotor, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	volatile systime_t time;

	/*** INFINITE LOOP ***/
	while(1){
		// Enters sleep mode if asked by another thread.
		if(ControlMotor_MetaData.Sleep){
			chSysLock();
			ControlMotor_MetaData.Sleep = chThdSuspendS(&ControlMotor_MetaData.ThdReference);
			chSysUnlock();
		}

		time = chVTGetSystemTime();

		// Checks if position left has been reached
		if(!PositionLeft_Reached){

			// Action when position left has been reached
			if(abs(left_motor_get_pos()) >= Position2Reach){
				PositionLeft_Reached = POSITION_REACHED;
				SpeedLeft = STOP_SPEED;
			}

			left_motor_set_speed(SpeedLeft);
		}

		// Checks if position right has been reached
		if(!PositionRight_Reached){

			// Action when position right has been reached
			if(abs(right_motor_get_pos()) >= Position2Reach){
				PositionRight_Reached = POSITION_REACHED;
				SpeedRight = STOP_SPEED;
			}

			right_motor_set_speed(SpeedRight);
		}

		// Signals semaphore when both positions have been reached
		if(PositionLeft_Reached && PositionRight_Reached){
			chBSemSignal(&MotorReady_sem);
		}

		// 1 kHz cycle because at high speed, position has to be checked faster
		chThdSleepUntilWindowed(time, time + MS2ST(1));
	}
	/*** END INFINITE LOOP ***/
}

/*** END INTERNAL FUNCTIONCS ***/

/*** PUBLIC FUNCTIONCS ***/

void control_motor_start(void){
	chThdCreateStatic(waControlMotor, sizeof(waControlMotor), NORMALPRIO+1, ControlMotor, NULL);
}

void correction_nominal_speed(int16_t SpeedCorrection){
	NominalSpeed += SpeedCorrection;
	if(NominalSpeed > SPEED_LIMIT_SUP){
		NominalSpeed = SPEED_LIMIT_SUP;
	}
	if(NominalSpeed < SPEED_LIMIT_INF){
		NominalSpeed = SPEED_LIMIT_INF;
	}
}

void turn(int16_t AngleVal){
	// Waits that the motors have reached their previous position before setting a new command.
	chBSemWait(&MotorReady_sem);

	// Resets left and right motors position
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	// Sets position to reach
	Position2Reach = abs(AngleVal);

	if(AngleVal > 0){					// turn right
		SpeedLeft = NominalSpeed;
		SpeedRight = -NominalSpeed;
	}else{								// turn left
		SpeedLeft = -NominalSpeed;
		SpeedRight = NominalSpeed;
	}

	/* Resets position reached (condition for Thd ControlMotor),
	 *  while in locked to start both motors at the same time
	 */
	chSysLock();
	PositionLeft_Reached = POSITION_NOT_REACHED;
	PositionRight_Reached = POSITION_NOT_REACHED;
	chSysUnlock();
}

void move(int16_t DistanceVal){
	// Waits that the motors have reached their previous position before setting a new command.
	chBSemWait(&MotorReady_sem);

	// Resets left and right motors position
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	// Sets position to reach
	Position2Reach = abs(DistanceVal);

	if(DistanceVal > 0){				// go forward
		SpeedLeft = NominalSpeed;
		SpeedRight = NominalSpeed;
	}else{								// go backward
		SpeedLeft = -NominalSpeed;
		SpeedRight = -NominalSpeed;
	}

	/* Resets position reached (condition for Thd ControlMotor),
	 *  while in locked to start both motors at the same time
	 */
	chSysLock();
	PositionLeft_Reached = POSITION_NOT_REACHED;
	PositionRight_Reached = POSITION_NOT_REACHED;
	chSysUnlock();
}

void go_next_cell(int16_t DirectionVal){
	// turn if necessary
	if(!(DirectionVal == MOVE_FORWARD)){
		turn(DirectionVal);
	}

	// move to next cell
	move(ONE_CELL);
}

/*** END PUBLIC FUNCTIONCS ***/
