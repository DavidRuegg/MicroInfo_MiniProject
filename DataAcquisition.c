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

/*** GLOBAL VARIABLES ***/

/*** STATIC VARIABLES ***/
static uint8_t cell_color;
static BSEMAPHORE_DECL(image_ready_sem, FALSE);

/*** THREADS ***/
static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 10, 220, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	po8030_set_awb(0);
	//po8030_set_rgb_gain(0x5E, 0x4F , 0x5D);
	//po8030_set_rgb_gain(0x55, 0x4F , 0x65);
	po8030_set_rgb_gain(0x52, 0x52 , 0x65);
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

static THD_WORKING_AREA(waProcessImage, 256);
static THD_FUNCTION(ProcessImage, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint8_t *img_buff_ptr;

	volatile uint32_t val_red = 0;
	volatile uint32_t val_green = 0;
	volatile uint32_t val_blue = 0;
	uint16_t max_val = 0;

	while(1){

		//waits until an image has been captured
		chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		val_red = 0;
		val_green = 0;
		val_blue = 0;
		cell_color = 0;

		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			val_red += (img_buff_ptr[i]&0xF8) >> 2;
			val_green += ((img_buff_ptr[i]&0x07) << 3) +
					((img_buff_ptr[i+1]&0xE0) >> 5);
			val_blue += (img_buff_ptr[i+1]&0x1F) << 1;
		}

		if(val_red >= val_green && val_red >= val_blue){
			max_val = val_red;
		}else if(val_green >= val_blue){
			max_val = val_green;
		}else{
			max_val = val_blue;
		}

		val_red = (val_red*100)/max_val;
		val_green = (val_green*100)/max_val;
		val_blue = (val_blue*100)/max_val;

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

		set_rgb_led(LED2, val_red, val_green, val_blue);
		set_rgb_led(LED8, val_red, val_green, val_blue);

	}
}

/*** PUBLIC FUNCTIONS ***/
void color_acquisition_start(void){
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
}

void scan_maze_cell(uint8_t* maze_cell){
	*maze_cell = 0;
	set_led(NUM_LED,0);
	for(uint8_t i=0 ; i<PROXIMITY_NB_CHANNELS ; i++){
		if((i == IR2) || (i == IR7)){
			continue;
		}
		else if(get_prox(i) > PROXIMITY_THRESHOLD){
			switch (i) {
			case IR1:
				*maze_cell |= WALL_FRONT_B;
				set_led(LED1, 1);
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

	*maze_cell |= cell_color;
	switch (*maze_cell & COLOR_B) {
	case YELLOW_B:
		set_rgb_led(LED4, 100, 100, 0);
		set_rgb_led(LED6, 100, 100, 0);
		break;
	case RED_B:
		set_rgb_led(LED4, 100, 0, 0);
		set_rgb_led(LED6, 100, 0, 0);
		break;
	case GREEN_B:
		set_rgb_led(LED4, 0, 100, 0);
		set_rgb_led(LED6, 0, 100, 0);
		break;
	case BLUE_B:
		set_rgb_led(LED4, 0, 0, 100);
		set_rgb_led(LED6, 0, 0, 100);
		break;
	case CYAN_B:
		set_rgb_led(LED4, 0, 100, 100);
		set_rgb_led(LED6, 0, 100, 100);
		break;
	case MAGENTA_B:
		set_rgb_led(LED4, 100, 0, 100);
		set_rgb_led(LED6, 100, 0, 100);
		break;
	case WHITE_B:
		set_rgb_led(LED4, 100, 100, 100);
		set_rgb_led(LED6, 100, 100, 100);
		break;
	default:
		set_rgb_led(LED4, 0, 0, 0);
		set_rgb_led(LED6, 0, 0, 0);
		break;
	}
}
