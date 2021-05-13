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

#define SEARCHING 0
#define FOUND 1
#define BLOCKED -1

#define TOGGLE_LED 42


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
 * @brief	Sets red LEDs according to wall detected by IR sensors.
 * 				Front wall --> LED1
 * 				Right wall --> LED3
 * 				Back wall --> LED5
 * 				Left wall --> LED7
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
void set_wall_leds(uint8_t maze_cell);

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
void set_floor_leds(uint8_t maze_cell);

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
void floor_color_action(uint8_t maze_cell);

/**
 * @brief
 */
void check_exit(uint8_t maze_cell, int8_t* OldStatus);

#endif /* DATAPROCESS_H_ */
