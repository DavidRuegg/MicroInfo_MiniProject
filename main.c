#include "hal.h"
#include "ch.h"
#include "memory_protection.h"

#include <sensors/proximity.h>
#include <camera/dcmi_camera.h>
#include <camera/po8030.h>
#include <motors.h>
#include <leds.h>
#include <spi_comm.h>

#include <main.h>
#include <DataAcquisition.h>
#include <DataProcess.h>
#include <SystemControl.h>

#include <selector.h>
/*** GLOBAL VARIABLES ***/
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

/*** EXTERN VARIABLES ***/
extern binary_semaphore_t motor_ready_sem;

int main(void){
	/*** INITIALIZATION ***/
	// init ChibiOS + mcu
	halInit();
	chSysInit();
	mpu_init();

	// init general bus message
	messagebus_init(&bus, &bus_lock, &bus_condvar);

	dcmi_start();
	po8030_start();

	motors_init();

	proximity_start();

	spi_comm_start();

	control_motor_start();
	color_acquisition_start();

	/*** INTERNAL VARIABLES ***/
	uint8_t actual_cell = 0;

	chThdSleepMilliseconds(2000);

	/*** Infinite loop. ***/
	while (1) {

		//	e-puck modes of operation
		switch(get_selector()){
		case POS_SEL_0:	//	selector = 0 --> left wall follower
			chThdSleepMilliseconds(1500);
			while(get_selector() == POS_SEL_0){
				scan_maze_cell(&actual_cell);
				move(left_wall_follower(actual_cell));
				chBSemWait(&motor_ready_sem);
			}
			break;
		case POS_SEL_1:	//	selector = 1 --> pledge
			reset_orientation();	//	so that pledge algo is usable without a total reset
			chThdSleepMilliseconds(1500);
			while(get_selector() == POS_SEL_1){
				scan_maze_cell(&actual_cell);
				move(pledge_algorithm(actual_cell));
				chBSemWait(&motor_ready_sem);
			}
			break;
		default:	//	default --> doesn't move
			scan_maze_cell(&actual_cell);
			break;
		}

		chThdYield();	//	!!!!! A PLACER DANS LES BOUCLES WHILE SI NECESSAIRE !!!!!
	}
	/* Infinite loop. */
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
	chSysHalt("Stack smashing detected");
}
