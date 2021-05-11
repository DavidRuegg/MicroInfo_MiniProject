/*
 * DataProcess.h
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#ifndef DATAPROCESS_H_
#define DATAPROCESS_H_

#define NB_ORIENTATION 4
#define NORTH	0
#define EAST	1
#define SOUTH	2
#define WEST	3


/**
 * @brief	Resets the value of the static variable orientation to 0.
 */
void reset_orientation(void);

/**
 * @brief	Returns the direction to go depending on the obstacles around the cell
 * 			 where the e-puck stands in the maze, according to the left-wall-follower
 * 			 rule.
 *
 * @param maze_cell		Bits 0 to 3 are set to 1 if the corresponding
 * 						 wall is around the e-puck.
 * 							Bit 0 --> front wall
 * 							Bit 1 --> right wall
 * 							Bit 2 --> back wall
 * 							Bit 3 --> left wall
 * 						Bits 4 to 6 are set to 1 according to the color of the floor.
 * 							Bit 4 --> blue
 * 							Bit 5 --> green
 * 							Bit 6 --> red
 *
 * @return				Value in steps corresponding to the number needed to turn
 * 						 right, left or backward. 0 if there is no need to turn.
 */
int16_t left_wall_follower(uint8_t maze_cell);

/**
 * @brief	Returns the direction to go depending on the obstacles around the cell
 * 			 where the e-puck stands in the maze, according to the pledge-algorithm
 * 			 rule.
 *
 * @param maze_cell		Bits 0 to 3 are set to 1 if the corresponding
 * 						 wall is around the e-puck.
 * 							Bit 0 --> front wall
 * 							Bit 1 --> right wall
 * 							Bit 2 --> back wall
 * 							Bit 3 --> left wall
 * 						Bits 4 to 6 are set to 1 according to the color of the floor.
 * 							Bit 4 --> blue
 * 							Bit 5 --> green
 * 							Bit 6 --> red
 *
 * @return				Value in steps corresponding to the number needed to turn
 * 						 right, left or backward. 0 if there is no need to turn.
 */
int16_t pledge_algorithm(uint8_t maze_cell);

/**
 * @brief	Sets RGB back LEDs according to color detected by the camera.
 *
 * @param maze_cell		Bits 0 to 3 are set to 1 if the corresponding
 * 						 wall is around the e-puck.
 * 							Bit 0 --> front wall
 * 							Bit 1 --> right wall
 * 							Bit 2 --> back wall
 * 							Bit 3 --> left wall
 * 						Bits 4 to 6 are set to 1 according to the color of the floor.
 * 							Bit 4 --> blue
 * 							Bit 5 --> green
 * 							Bit 6 --> red
 */
void color_action(uint8_t maze_cell);

#endif /* DATAPROCESS_H_ */
