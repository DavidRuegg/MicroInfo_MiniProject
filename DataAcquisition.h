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
#define PROXIMITY_THRESHOLD 140 // experimental value

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
 * @brief
 */
void proximity_acquisition_start(void);

/**
 * @brief	Starts thread to capture the color of the floor with
 * 			 NORMALPRIO to CaptureImage and ProcessImage
 */
void color_acquisition_start(void);

/**
 * @brief
 */
uint8_t get_actual_cell(void);

/*
void make_thread_captureImage_sleep(void);

void make_thread_captureImage_wakeup(void);

void make_thread_getProximity_sleep(void);

void make_thread_getProximity_wakeup(void);
*/
#endif /* DATAACQUISITION_H_ */
