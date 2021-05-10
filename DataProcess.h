/*
 * DataProcess.h
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#ifndef DATAPROCESS_H_
#define DATAPROCESS_H_

#define WALL_B 0x0F

#define NORTH	0
#define EAST	1
#define SOUTH	2
#define WEST	3

/**
 * @brief
 *
 * @param maze_cell		the 4 lower bits are set to 1 if the corresponding
 * 						wall is around the e-puck in this order from the lower
 * 						to the higher : front, right, back, left.
 * @param orientation
 */
void change_frame_reference(uint8_t* maze_cell, uint8_t orientation);

/**
 * @brief	reset the value of the static variable orientation to 0
 */
void reset_orientation(void);

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
int16_t left_wall_follower(uint8_t maze_cell);

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
int16_t pledge_algorithm(uint8_t maze_cell);

#endif /* DATAPROCESS_H_ */
