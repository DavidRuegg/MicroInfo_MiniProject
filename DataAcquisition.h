/**
 * @file	DataAcquisition.h
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	16.05.2021
 *
 * @brief	Public prototypes of function for object and color detection.
 * 			Define for IR sensors and camera settings.
 */

#ifndef DATAACQUISITION_H_
#define DATAACQUISITION_H_

// Proximity define
#define IR1 				0
#define IR2 				1
#define IR3 				2
#define IR4 				3
#define IR5 				4
#define IR6 				5
#define IR7 				6
#define IR8 				7
#define PROXIMITY_THRESHOLD 120		// experimental value

// Image define
#define IMAGE_BUFFER_SIZE 	200		// size of a row [pxl]
#define COLOR_THRESHOLD 	66		// two third of maximal value


/**
 * @brief	Starts thread to detect wall around the e-puck with
 * 			 NORMALPRIO to GetProximity
 */
void proximity_acquisition_start(void);

/**
 * @brief	Starts thread to capture the color of the floor with
 * 			 NORMALPRIO to CaptureImage and ProcessImage
 */
void color_acquisition_start(void);

/**
 * @brief	Get the static variable ActualCell with the most recent data
 *
 * @return ActualCell	Bits 0 to 3 are set to 1 if the corresponding
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
uint8_t get_actual_cell(void);

#endif /* DATAACQUISITION_H_ */
