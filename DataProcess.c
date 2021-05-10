/*
 * DataProcess.c
 *
 *  Created on: 26 avr. 2021
 *      Author: druegg
 */

#include <stdint.h>

#include <main.h>
#include <DataProcess.h>
#include <DataAcquisition.h>

/*** INTERNAL FUNCTIONS ***/
void change_frame_reference(uint8_t* maze_cell, uint8_t orientation){
	uint8_t maze_cell_tmp1 = (*maze_cell & WALL_B) << orientation;
	uint8_t maze_cell_tmp2 = (maze_cell_tmp1 & 0x0F) | (maze_cell_tmp1 >> 4);
	*maze_cell = (*maze_cell & 0xF0) | maze_cell_tmp2;
}
