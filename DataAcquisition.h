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
#define PROXIMITY_THRESHOLD 150 // experimental value

#define COLOR_B 0x70
#define BLUE_B 0x10
#define GREEN_B 0x20
#define CYAN_B 0x30
#define RED_B 0x40
#define MAGENTA_B 0x50
#define YELLOW_B 0x60
#define WHITE_B 0x70
#define COLOR_THRESHOLD 66

#define HIGH_CELL_BITS 0xF0

#define IMAGE_BUFFER_SIZE 200


/**
 * @brief	Starts thread to capture the color of the floor with
 * 			 NORMALPRIO to CaptureImage and ProcessImage
 */
void color_acquisition_start(void);

/**
 * @brief	Changes the value of the variable maze_cell according to the presence
 * 			 of obstacles and floor color.
 * 			Sets red LEDs according to the presence of obstacles around the e-puck.
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
void scan_maze_cell(uint8_t* maze_cell);

#endif /* DATAACQUISITION_H_ */
