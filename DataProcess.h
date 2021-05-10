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

#define COLOR_B 0x70
#define BLUE_B 0x10
#define GREEN_B 0x20
#define CYAN_B 0x30
#define RED_B 0x40
#define MAGENTA_B 0x50
#define YELLOW_B 0x60
#define WHITE_B 0x70
#define COLOR_THRESHOLD 66

void change_frame_reference(uint8_t* maze_cell, uint8_t orientation);
uint8_t get_color(void);
void process_image_start(void);

#endif /* DATAPROCESS_H_ */
