/*
 * DataProcess.c
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#include <stdint.h>

#include <leds.h>

#include <main.h>
#include <DataProcess.h>
#include <DataAcquisition.h>
#include <SystemControl.h>

/*** STATIC VARIABLES ***/

/* Keeps track of the orientation of the e-puck all the time
 * used in pledge and lwf (useless in lwf if used without pledge) and
 * to change the reference frame (e-puck frame or maze frame).
 */
static int8_t orientation = 0;

/*** INTERNAL FUNCTIONS ***/

/**
 * @brief	Converts wall position from e-puck reference frame to
 * 			 maze reference frame.
 *
 * @param cell_epuck	Cell in epuck reference frame.
 * 						Bits 0 to 3 are set to 1 if the corresponding
 * 						 wall is around the e-puck.
 * 							Bit 0 --> front wall
 * 							Bit 1 --> right wall
 * 							Bit 2 --> back wall
 * 							Bit 3 --> left wall
 *
 * @return cell_maze	Cell in maze reference frame.
 * 						Bits 0 to 3 are set to 1 if the corresponding
 * 						 wall is around the e-puck.
 * 							Bit 0 --> north wall
 * 							Bit 1 --> east wall
 * 							Bit 2 --> south wall
 * 							Bit 3 --> west wall
 */
uint8_t change_frame_reference(uint8_t cell_epuck){
	// Copies only wall bits
	uint8_t cell_maze = (cell_epuck & WALL_B);

	// Re-defines the orientation to a maximum of one complete turn
	int8_t orientation_tmp = orientation % NB_ORIENTATION;

	if(orientation < 0){					// ACW orientation
		cell_maze <<= (4 + orientation_tmp);
	}else{									// CW orientation
		cell_maze <<= orientation_tmp;
	}

	// Returns cell in maze reference frame, with original higher bits
	return ((cell_maze & WALL_B) | (cell_maze >> 4) | (cell_epuck & HIGH_CELL_BITS));
}

/*** END INTERNAL FUNCTIONS ***/

/*** PUBLIC FUNCTIONS ***/

void reset_orientation(void){
	orientation = 0;
}

int16_t left_wall_follower(uint8_t maze_cell){		//	lwf algorithm
	if(!(maze_cell & WALL_LEFT_B)){					//	turn left possible --> turn left
		orientation--;
		return LEFT_TURN;
	}else if(!(maze_cell & WALL_FRONT_B)){			//	...impossible --> move forward if possible
		return MOVE_FORWARD;
	}else if(!(maze_cell & WALL_RIGHT_B)){			//	...impossible --> turn right if possible
		orientation++;
		return RIGHT_TURN;
	}else{											//	...impossible --> turn backward
		orientation += 2;
		return BACKWARD_TURN;
	}
}

int16_t pledge_algorithm(uint8_t maze_cell){
	if(orientation == 0){							//	pledge
		if(!(maze_cell & WALL_FRONT_B)){			//	move forward possible --> move forward
			return MOVE_FORWARD;
		}else if(!(maze_cell & WALL_RIGHT_B)){		//	...impossible --> turn right if possible
			orientation++;
			return RIGHT_TURN;
		}else{										//	...impossible --> turn backward
			orientation += 2;
			return BACKWARD_TURN;
		}
	}else{											//	lwf
		return left_wall_follower(maze_cell);
	}
}

void color_action(uint8_t maze_cell){
	switch (maze_cell & COLOR_B) {
	case YELLOW_B:
		set_rgb_led(LED4, 100, 100, 0);
		set_rgb_led(LED6, 100, 100, 0);
		break;
	case RED_B:
		set_rgb_led(LED4, 100, 0, 0);
		set_rgb_led(LED6, 100, 0, 0);
		break;
	case GREEN_B:
		set_rgb_led(LED4, 0, 100, 0);
		set_rgb_led(LED6, 0, 100, 0);
		//correction_nominal_speed(CORRECTION_SPEED);
		break;
	case BLUE_B:
		set_rgb_led(LED4, 0, 0, 100);
		set_rgb_led(LED6, 0, 0, 100);
		break;
	case CYAN_B:
		set_rgb_led(LED4, 0, 100, 100);
		set_rgb_led(LED6, 0, 100, 100);
		break;
	case MAGENTA_B:
		set_rgb_led(LED4, 100, 0, 100);
		set_rgb_led(LED6, 100, 0, 100);
		break;
	case WHITE_B:
		set_rgb_led(LED4, 100, 100, 100);
		set_rgb_led(LED6, 100, 100, 100);
		break;
	default:
		set_rgb_led(LED4, 0, 0, 0);
		set_rgb_led(LED6, 0, 0, 0);
		break;
	}
}

/*** END PUBLIC FUNCTIONS ***/
