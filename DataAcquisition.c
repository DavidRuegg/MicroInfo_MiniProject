/*
 * DataAcquisition.c
 *
 *  Created on: 23 avr. 2021
 *      Author: druegg
 */
#include <stdint.h>

#include "ch.h"

#include <camera/po8030.h>
#include <camera/dcmi_camera.h>
#include <sensors/proximity.h>
#include <leds.h>

#include <DataAcquisition.h>
#include <DataProcess.h>

/*** STATIC VARIABLES ***/

static uint8_t cell_color;
static BSEMAPHORE_DECL(image_ready_sem, FALSE);

/*** INTERNAL FUNCTIONS ***/

/**
 * @brief	Thread which configures and captures images.
 * 			Signals semaphore image_ready_sem when an image has been captured.
 */
static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg){
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	// Image configuration: format --> RGB565, origin --> (220,240), size --> (IMAGE_BUFFER_SIZE, 2)
	po8030_advanced_config(FORMAT_RGB565, 220, 240, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);

	// White balance disabled in order to identify the colors
	po8030_set_awb(0);

	// RGB gain adjusted to the scene
	po8030_set_rgb_gain(0x52, 0x52 , 0x65);
	//po8030_set_rgb_gain(0x5E, 0x4F , 0x5D);
	//po8030_set_rgb_gain(0x55, 0x4F , 0x65);

	// Contrast adjusted to the scene
	po8030_set_contrast(20);

	// Double buffering enabled in order to process image while capturing another
	dcmi_enable_double_buffering();

	// Capture mode set to one shot
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);

	// Prepares DCMI units
	dcmi_prepare();

	/*** INFINITE LOOP ***/
	while(1){
		// Starts a capture
		dcmi_capture_start();

		// Waits for the capture to be done
		wait_image_ready();

		// Signals an image has been captured
		chBSemSignal(&image_ready_sem);
	}
	/*** END INFINITE LOOP ***/
}

/**
 * @brief	Thread which extracts the colors of the image.
 * 			Sets the colors to RGB front LEDs.
 * 			Sets the colors to static variable cell_color.
 */
static THD_WORKING_AREA(waProcessImage, 256);
static THD_FUNCTION(ProcessImage, arg){
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	/*** INTERNAL VARIABLES ***/

	uint8_t *img_buff_ptr = NULL;

	uint32_t val_red = 0;
	uint32_t val_green = 0;
	uint32_t val_blue = 0;
	uint16_t max_val = 0;

	/*** INFINITE LOOP ***/
	while(1){
		// Waits until an image has been captured
		chBSemWait(&image_ready_sem);

		// Gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		// Resets values of last image
		val_red = 0;
		val_green = 0;
		val_blue = 0;
		cell_color = 0;

		// Extracts and adds all pixels values of one line (format RGB565)
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			val_red += (img_buff_ptr[i]&0xF8) >> 2;
			val_green += ((img_buff_ptr[i]&0x07) << 3) +
					((img_buff_ptr[i+1]&0xE0) >> 5);
			val_blue += (img_buff_ptr[i+1]&0x1F) << 1;
		}

		// Checks for the maximum value of RGB
		if(val_red >= val_green && val_red >= val_blue){
			max_val = val_red;
		}else if(val_green >= val_blue){
			max_val = val_green;
		}else{
			max_val = val_blue;
		}

		// Sets scale to percentage of maximum value
		val_red = (val_red*100)/max_val;
		val_green = (val_green*100)/max_val;
		val_blue = (val_blue*100)/max_val;

		/* Saves the colors to static variable color cell
		 *  system is locked in order to not mix colors with the previous ones
		 */
		chSysLock();
		if(val_red > COLOR_THRESHOLD){
			cell_color |= RED_B;
		}
		if(val_green > COLOR_THRESHOLD){
			cell_color |= GREEN_B;
		}
		if(val_blue > COLOR_THRESHOLD){
			cell_color |= BLUE_B;
		}
		chSysUnlock();

		// Sets camera output to RGB front LEDs
		set_rgb_led(LED2, val_red, val_green, val_blue);
		set_rgb_led(LED8, val_red, val_green, val_blue);
	}
	/*** END INFINITE LOOP ***/
}

/*** END INTERNAL FUNCTIONS ***/

/*** PUBLIC FUNCTIONS ***/

void color_acquisition_start(void){
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
}

void scan_maze_cell(uint8_t* maze_cell){
	// Resets cell
	*maze_cell = 0;

	/*** SCAN FOR WALLS ***
	 * Walls are saved on bits 0 to 3
	 * 	IR1 and IR8 --> front wall
	 * 	IR3 		--> right wall
	 * 	IR4 and IR5 --> back wall
	 * 	IR6			--> left wall
	 * Red LEDs are set to indicate a wall
	 */
	set_led(NUM_LED,0);
	for(uint8_t i=0 ; i<PROXIMITY_NB_CHANNELS ; i++){
		if((i == IR2) || (i == IR7)){	// no use of IR2 and IR7 sensors
			continue;
		}
		else if(get_prox(i) > PROXIMITY_THRESHOLD){
			switch (i) {
			case IR1:
			case IR8:
				*maze_cell |= WALL_FRONT_B;
				set_led(LED1, 1);
				break;
			case IR3:
				*maze_cell |= WALL_RIGHT_B;
				set_led(LED3, 1);
				break;
			case IR4:
			case IR5:
				*maze_cell |= WALL_BACK_B;
				set_led(LED5, 1);
				break;
			case IR6:
				*maze_cell |= WALL_LEFT_B;
				set_led(LED7, 1);
				break;
			default:
				break;
			}
		}
	}
	/*** END SCAN FOR WALLS ***/

	/*** SCAN FOR FLOOR COLOR ***
	 * Color of the floor is saved on bits 4 to 6
	 * 	Bit 4 --> blue
	 * 	Bit 5 --> green
	 *  Bit 6 --> red
	 */
	*maze_cell |= cell_color;
	/*** END SCAN FOR FLOOR COLOR ***/
}

/*** END PUBLIC FUNCTIONS ***/
