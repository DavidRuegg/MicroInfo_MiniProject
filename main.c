#include "hal.h"
#include "ch.h"
#include "memory_protection.h"

#include <sensors/proximity.h>
#include <camera/dcmi_camera.h>
#include <camera/po8030.h>
#include <motors.h>
#include <leds.h>
#include <selector.h>
#include <spi_comm.h>

#include <main.h>
#include <DataAcquisition.h>
#include <DataProcess.h>
#include <SystemControl.h>

/*** GLOBAL VARIABLES ***/
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

/*** EXTERN VARIABLES ***/
extern binary_semaphore_t motor_ready_sem;

int main(void){
	/*** INTERNAL VARIABLES ***/
	uint8_t actual_cell = 0;

	/*** INITIALISATION ***/
	// inits ChibiOS + mcu
	halInit();
	chSysInit();
	mpu_init();

	// inits intern communication protocol
	messagebus_init(&bus, &bus_lock, &bus_condvar);

	// inits peripherals
	dcmi_start();
	po8030_start();
	motors_init();
	proximity_start();
	spi_comm_start();

	// inits threads
	control_motor_start();
	color_acquisition_start();

	// sleeps to get everything correctly initialisated
	chThdSleepMilliseconds(2000);

	/*** INFINITE LOOP ***/
	while(1){

		/*** E-PUCK MODES OF OPERATION ***/
		switch(get_selector()){
		case POS_SEL_0:		// selector = 0 --> left wall follower
			chThdSleepMilliseconds(1500);
			while(get_selector() == POS_SEL_0){
				scan_maze_cell(&actual_cell);
				color_action(actual_cell);
				move(left_wall_follower(actual_cell));
				chBSemWait(&motor_ready_sem);
			}
			break;
		case POS_SEL_1:		// selector = 1 --> pledge
			reset_orientation();	// so that pledge algorithm is usable without a total reset
			chThdSleepMilliseconds(1500);
			while(get_selector() == POS_SEL_1){
				scan_maze_cell(&actual_cell);
				color_action(actual_cell);
				move(pledge_algorithm(actual_cell));
				chBSemWait(&motor_ready_sem);
			}
			break;
		default:			// default --> doesn't move
			scan_maze_cell(&actual_cell);
			color_action(actual_cell);
			break;
		}

		chThdYield();
	}
	/*** END INFINITE LOOP ***/
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
	chSysHalt("Stack smashing detected");
}
