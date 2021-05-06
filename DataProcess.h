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
void process_image_start(void);

#endif /* DATAPROCESS_H_ */
