/*
 * DataAcquisition.c
 *
 *  Created on: 23 avr. 2021
 *      Author: druegg
 */
#include <stdint.h>

#include <DataAcquisition.h>

#include <sensors/proximity.h>
#include <leds.h>

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


