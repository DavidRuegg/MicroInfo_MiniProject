/**
 * @file	SystemControl.h
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	16.05.2021
 *
 * @brief	Public prototypes of function to control movement of an e-puck robot.
 * 			Define for speed and fixed movements.
 */

#ifndef SYSTEMCONTROL_H_
#define SYSTEMCONTROL_H_

/*** MOTOR DEFINE ***/
// Speed define
#define NOMINAL_SPEED			500		// in [step/s]
#define CORRECTION_SPEED		100		// in [step/s]
#define STOP_SPEED				0		// in [step/s]
#define SPEED_LIMIT_SUP			800		// in [step/s]
#define SPEED_LIMIT_INF			200		// in [step/s]
// Movement define
#define NSTEP_ONE_REVOLUTION	1000	// steps for a complete revolution
#define MOVE_FORWARD			0		// continue forward, no turn
#define MM_2_STEP				7.6935	// converts millimeters to steps (experimental)
#define ONE_CELL 				884 	// steps for one cell (experimental)
#define DEGREE_2_STEP			3.6069	// converts degrees to steps (experimental)
#define LEFT_TURN				-324	// steps for 90 degree turn left (experimental)
#define RIGHT_TURN				324 	// steps for 90 degree turn right (experimental)
#define BACKWARD_TURN			648 	// steps for 180 degree turn (experimental)
#define ONE_TURN				-1298 	// steps for 180 degree turn (experimental)
// State define
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
 * @param SpeedCorrection	Value in steps per seconds to increase/decrease the nominal speed
 * 					 		 Positive value to increase.
 * 					 		 Negative value to decrease.
 */
void correction_nominal_speed(int16_t SpeedCorrection);

/**
 * @brief	Sets the position to reach for each motor at nominal speed
 * 			 for the e-puck to do a turn.
 *
 * @param AngleVal		Value in steps corresponding to the number needed to do the turn.
 * 					 	 Positive value to turn right.
 * 					 	 Negative value to turn left.
 */
void turn(int16_t AngleVal);

/**
 * @brief	Sets the position to reach for each motor at nominal speed
 * 			 for the e-puck to move of a certain distance.
 *
 * @param DistanceVal	Value in steps corresponding to the number needed to move the expected length.
 * 						 Positive value to go forward.
 * 						 Negative value to go backward.
 */
void move(int16_t DistanceVal);

/**
 * @brief	Sets the position to reach for each motor at nominal speed,
 * 			 for the e-puck to turn and then move forward or only move forward
 * 			 for a fixed distance of one cell of the maze.
 *
 * @param DirectionVal	Value in steps corresponding to the number needed to do the turn,
 * 						 Positive value to turn right then move forward.
 * 						 Negative to turn left then move forward.
 * 						 0 if only move forward.
 */
void go_next_cell(int16_t DirectionVal);

#endif /* SYSTEMCONTROL_H_ */
