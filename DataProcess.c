/*
 * DataProcess.c
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#include <stdint.h>

#include <main.h>
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

/**
 * @brief	reset the value of the static variable orientation to 0
 */
void reset_orientation(void){
	orientation = 0;
}

/**
 * @brief	Returns the direction to go depending on the obstacles around the cell
 * 			where the e-puck stands in the maze, according to the left-wall-follower
 * 			rule.
 *
 * @param maze_cell		the 4 lower bits are set to 1 if the corresponding
 * 						wall is around the e-puck in this order from the lower
 * 						to the higher : front, right, back, left.
 *
 * @return				Value in steps corresponding to the number needed to turn
 * 						right, left or backward. 0 if there is no need to turn.
 */
int16_t left_wall_follower(uint8_t maze_cell){			//	lwf algorithm

	if(!(maze_cell & WALL_LEFT_B)){						//	turn left possible --> turn left
		-- orientation;
		return LEFT_TURN;
	}else if(!(maze_cell & WALL_FRONT_B)){				//	...impossible --> move forward if possible
		return MOVE_FORWARD;
	}else if(!(maze_cell & WALL_RIGHT_B)){				//	...impossible --> turn right if possible
		++ orientation;
		return RIGHT_TURN;
	}else{											//	...impossible --> turn backward
		orientation = orientation + 2;
		return BACKWARD_TURN;
	}
}

/**
 * @brief	Returns the direction to go depending on the obstacles around the cell
 * 			where the e-puck stands in the maze, according to the pledge-algorithm
 * 			rule.
 *
 * @param maze_cell		the 4 lower bits are set to 1 if the corresponding
 * 						wall is around the e-puck in this order from the lower
 * 						to the higher : front, right, back, left.
 *
 * @return				Value in steps corresponding to the number needed to turn
 * 						right, left or backward. 0 if there is no need to turn.
 */
int16_t pledge_algorithm(uint8_t maze_cell){

	if(orientation == 0){							//	pledge
		if(!(maze_cell & WALL_FRONT_B)){			//	move forward possible --> move forward
			return MOVE_FORWARD;
		}else if(!(maze_cell & WALL_RIGHT_B)){		//	...impossible --> turn right if possible
			++ orientation;
			return RIGHT_TURN;
		}else{										//	...impossible --> turn backward
			orientation = orientation + 2;
			return BACKWARD_TURN;
		}
	}else{											//	lwf
		return left_wall_follower(maze_cell);
	}

}




