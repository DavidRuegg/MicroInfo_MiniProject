/*
 * DataAcquisition.h
 *
 *  Created on: 23 avr. 2021
 *      Author: druegg
 */

#ifndef DATAACQUISITION_H_
#define DATAACQUISITION_H_

#define WALL_B 0x0F
#define WALL_FRONT_B 0x01
#define WALL_RIGHT_B 0x02
#define WALL_BACK_B 0x04
#define WALL_LEFT_B 0x08

#define IR1 0
#define IR2 1
#define IR3 2
#define IR4 3
#define IR5 4
#define IR6 5
#define IR7 6
#define IR8 7
#define PROXIMITY_THRESHOLD 120 // experimental value

#define COLOR_B 0x70
#define BLUE_B 0x10
#define GREEN_B 0x20
#define CYAN_B 0x30
#define RED_B 0x40
#define MAGENTA_B 0x50
#define YELLOW_B 0x60
#define WHITE_B 0x70
#define COLOR_THRESHOLD 66

#define IMAGE_BUFFER_SIZE 200

void color_acquisition_start(void);

/**
 * @brief	Changes the value of the variable maze_cell and sets 4 leds
 * 			according to the presence of obstacles around the e-puck.
 *
 * @param maze_cell		the 4 lower bits are set to 1 if the corresponding
 * 						wall is around the e-puck in this order from the lower
 * 						to the higher : front, right, back, left.
 */
void scan_maze_cell(uint8_t* maze_cell);

#endif /* DATAACQUISITION_H_ */
