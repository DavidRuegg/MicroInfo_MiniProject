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

//*** GLOBAL VARIABLES ***/
BSEMAPHORE_DECL(image_ready_sem, FALSE);

void scan_maze_cell(uint8_t* maze_cell){
	*maze_cell = 0;
	set_led(NUM_LED,0);
	for(uint8_t i=0 ; i<PROXIMITY_NB_CHANNELS ; i++){
		if (get_prox(i) > PROXIMITY_THRESHOLD){
			switch (i) {
			case IR1:
				*maze_cell |= WALL_FRONT_B;
				set_led(LED1, 1);
				break;
			case IR2:
				// WHAT TO DO?
				break;
			case IR3:
				*maze_cell |= WALL_RIGHT_B;
				set_led(LED3, 1);
				break;
			case IR4:
				*maze_cell |= WALL_BACK_B;
				set_led(LED5, 1);
				break;
			case IR5:
				*maze_cell |= WALL_BACK_B;
				set_led(LED5, 1);
				break;
			case IR6:
				*maze_cell |= WALL_LEFT_B;
				set_led(LED7, 1);
				break;
			case IR7:
				// WHAT TO DO?
				break;
			case IR8:
				*maze_cell |= WALL_FRONT_B;
				set_led(LED1, 1);
				break;
			default:
				//chprintf
				break;
			}
		}
	}
}


//** WARNING FROM TP4 (sources!) **//
static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 10, 220, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
    po8030_set_awb(0);
    po8030_set_rgb_gain(0x5E, 0x4F , 0x5D);
    po8030_set_contrast(20);
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);
    }
}

void capture_image_start(void){
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}


