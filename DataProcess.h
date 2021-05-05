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

void change_frame_reference(uint8_t* maze_cell, uint8_t orientation);
int16_t left_wall_follower(uint8_t cell); 										// Return a direction in steps
int16_t pledge_algorithm(uint8_t cell);											// or 0 if no need to turn

#endif /* DATAPROCESS_H_ */
