/*
 * DataProcess.c
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#include <stdint.h>
#include <stdbool.h>

#include "ch.h"

#include <camera/dcmi_camera.h>
#include <leds.h>

#include <main.h>
#include <DataProcess.h>
#include <DataAcquisition.h>

/*** EXTERN VARIABLES ***/
extern binary_semaphore_t image_ready_sem;

void change_frame_reference(uint8_t* maze_cell, uint8_t orientation){
	uint8_t maze_cell_tmp1 = (*maze_cell & WALL_B) << orientation;
	uint8_t maze_cell_tmp2 = (maze_cell_tmp1 & 0x0F) | (maze_cell_tmp1 >> 4);
	*maze_cell = (*maze_cell & 0xF0) | maze_cell_tmp2;
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
/*
	volatile uint8_t mean_red = 0;
	volatile uint8_t mean_green = 0;
	volatile uint8_t mean_blue = 0;
*/
	while(1){

		//waits until an image has been captured
		chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		val_red = 0;
		val_green = 0;
		val_blue = 0;

		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){
			val_red += (img_buff_ptr[i]&0xF8) >> 2;
			val_green += ((img_buff_ptr[i]&0x07) << 3) +
					((img_buff_ptr[i+1]&0xE0) >> 5);
			val_blue += (img_buff_ptr[i+1]&0x1F) << 1;
		}
/*
		mean_red = val_red/IMAGE_BUFFER_SIZE;
		mean_green = val_green/IMAGE_BUFFER_SIZE;
		mean_blue = val_blue/IMAGE_BUFFER_SIZE;
*/
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

		set_rgb_led(LED2, val_red, val_green, val_blue);
		set_rgb_led(LED4, val_red, val_green, val_blue);
		set_rgb_led(LED6, val_red, val_green, val_blue);
		set_rgb_led(LED8, val_red, val_green, val_blue);
	}
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
}
