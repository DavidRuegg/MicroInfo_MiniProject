/*
 * DataAcquisition.h
 *
 *  Created on: 23 avr. 2021
 *      Author: druegg
 */

#ifndef DATAACQUISITION_H_
#define DATAACQUISITION_H_

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
#define PROXIMITY_THRESHOLD 100 // experimental value


/**
 * @brief	Changes the value of the variable maze_cell and sets 4 leds
 * 			according to the presence of obstacles around the e-puck.
 *
 * @param maze_cell		the 4 lower bits are set to 1 if the corresponding
 * 						wall is around the e-puck in this order from the lower
 * 						to the higher : front, right, back, left.
 */
void scan_maze_cell(uint8_t* maze_cell);

/*
typedef struct {
	uint8_t	maze_cell
} maze_cell_t;
*/

#endif /* DATAACQUISITION_H_ */
