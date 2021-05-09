/*
 * DataProcess.c
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#include <stdint.h>

#include <DataProcess.h>
#include <DataAcquisition.h>
#include <SystemControl.h>

// keeps track of the orientation of the e-puck all the time
// used in pledge and lwf (useless in lwf if used without pledge)
static int8_t orientation = 0;

void change_frame_reference(uint8_t* maze_cell, uint8_t orientation){
	uint8_t maze_cell_tmp1 = (*maze_cell & WALL_B) << orientation;
	uint8_t maze_cell_tmp2 = (maze_cell_tmp1 & 0x0F) | (maze_cell_tmp1 >> 4);
	*maze_cell = (*maze_cell & 0xF0) | maze_cell_tmp2;
}

void reset_orientation(void){
	orientation = 0;
}

int16_t left_wall_follower(uint8_t cell){			//	lwf algorithm

	if(!(cell & WALL_LEFT_B)){						//	turn left possible --> turn left
		-- orientation;
		return LEFT_TURN;
	}else if(!(cell & WALL_FRONT_B)){				//	...impossible --> move forward if possible
		return MOVE_FORWARD;
	}else if(!(cell & WALL_RIGHT_B)){				//	...impossible --> turn right if possible
		++ orientation;
		return RIGHT_TURN;
	}else{											//	...impossible --> turn backward
		orientation = orientation + 2;
		return BACKWARD_TURN;
	}
}

int16_t pledge_algorithm(uint8_t cell){

	if(orientation == 0){							//	pledge
		if(!(cell & WALL_FRONT_B)){					//	move forward possible --> move forward
			return MOVE_FORWARD;
		}else if(!(cell & WALL_RIGHT_B)){			//	...impossible --> turn right if possible
			++ orientation;
			return RIGHT_TURN;
		}else{										//	...impossible --> turn backward
			orientation = orientation + 2;
			return BACKWARD_TURN;
		}
	}else{											//	lwf
		return left_wall_follower(cell);
	}

}




