/**
 * @file	DataProcess.h
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	16.05.2021
 *
 * @brief	Public prototypes of functions used to solve a maze and actions.
 */

#ifndef DATAPROCESS_H_
#define DATAPROCESS_H_

/**
 * @brief	Resets the value of the static variable EPuckOrientation to 0.
 */
void reset_orientation(void);

/**
 * @brief	Returns the direction to go depending on the obstacles around the cell
 * 			 where the e-puck stands in the maze, according to the left-wall-follower
 * 			 rule.
 *
 * @param Cell_Ref_EPuck	Bits 0 to 3 are set to 1 if the corresponding
 * 							 wall is around the e-puck.
 * 								Bit 0 --> front wall
 * 								Bit 1 --> right wall
 * 								Bit 2 --> back wall
 * 								Bit 3 --> left wall
 * 							Bits 4 to 6 are set to 1 according to the color of the floor.
 * 								Bit 4 --> blue
 * 								Bit 5 --> green
 * 								Bit 6 --> red
 *
 * @return					Value in steps corresponding to the number needed to turn
 * 							 right, left or backward. 0 if there is no need to turn.
 */
int16_t left_wall_follower(uint8_t Cell_Ref_EPuck);

/**
 * @brief	Returns the direction to go depending on the obstacles around the cell
 * 			 where the e-puck stands in the maze, according to the pledge-algorithm
 * 			 rule.
 *
 * @param Cell_Ref_EPuck	Bits 0 to 3 are set to 1 if the corresponding
 * 							 wall is around the e-puck.
 * 								Bit 0 --> front wall
 * 								Bit 1 --> right wall
 * 								Bit 2 --> back wall
 * 								Bit 3 --> left wall
 * 							Bits 4 to 6 are set to 1 according to the color of the floor.
 * 								Bit 4 --> blue
 * 								Bit 5 --> green
 * 								Bit 6 --> red
 *
 * @return					Value in steps corresponding to the number needed to turn
 * 							 right, left or backward. 0 if there is no need to turn.
 */
int16_t pledge_algorithm(uint8_t Cell_Ref_EPuck);

/**
 * @brief	Sets red LEDs according to wall detected by IR sensors.
 * 				Front wall --> LED1
 * 				Right wall --> LED3
 * 				Back wall --> LED5
 * 				Left wall --> LED7
 *
 * @param Cell_Ref_EPuck	Bits 0 to 3 are set to 1 if the corresponding
 * 							 wall is around the e-puck.
 * 								Bit 0 --> front wall
 * 								Bit 1 --> right wall
 * 								Bit 2 --> back wall
 * 								Bit 3 --> left wall
 * 							Bits 4 to 6 are set to 1 according to the color of the floor.
 * 								Bit 4 --> blue
 * 								Bit 5 --> green
 * 								Bit 6 --> red
 */
void set_wall_leds(uint8_t Cell_Ref_EPuck);

/**
 * @brief	Sets RGB back LEDs according to color detected by the camera.
 *
 * @param Cell_Ref_EPuck	Bits 0 to 3 are set to 1 if the corresponding
 * 							 wall is around the e-puck.
 * 								Bit 0 --> front wall
 * 								Bit 1 --> right wall
 * 								Bit 2 --> back wall
 * 								Bit 3 --> left wall
 * 							Bits 4 to 6 are set to 1 according to the color of the floor.
 * 								Bit 4 --> blue
 * 								Bit 5 --> green
 * 								Bit 6 --> red
 */
void set_floor_leds(uint8_t Cell_Ref_EPuck);

/**
 * @brief	Action on the e-puck according to color detected by the camera.
 * 				Red:	increase speed
 * 				Green:	decrease speed
 * 				Blue:	spin on itself
 *
 * @param Cell_Ref_EPuck	Bits 0 to 3 are set to 1 if the corresponding
 * 							 wall is around the e-puck.
 * 								Bit 0 --> front wall
 * 								Bit 1 --> right wall
 * 								Bit 2 --> back wall
 * 								Bit 3 --> left wall
 * 							Bits 4 to 6 are set to 1 according to the color of the floor.
 * 								Bit 4 --> blue
 * 								Bit 5 --> green
 * 								Bit 6 --> red
 */
void floor_color_action(uint8_t Cell_Ref_EPuck);

/**
 * @brief	Checks if the exit has been found.
 *
 * @param [in] Cell_Ref_EPuck	Bits 0 to 3 are set to 1 if the corresponding
 * 								 wall is around the e-puck.
 * 									Bit 0 --> front wall
 * 									Bit 1 --> right wall
 * 									Bit 2 --> back wall
 * 									Bit 3 --> left wall
 * 								Bits 4 to 6 are set to 1 according to the color of the floor.
 * 									Bit 4 --> blue
 * 									Bit 5 --> green
 * 									Bit 6 --> red
 *
 * @param [in/out] EPuckStatus	Previous and New status of the e-puck.
 */
void check_exit(uint8_t Cell_Ref_EPuck, int8_t* EPuckStatus);

#endif /* DATAPROCESS_H_ */
