#include "hal.h"
#include "ch.h"
#include "memory_protection.h"

#include <main.h>
#include <DataAcquisition.h>
#include <DataProcess.h>
#include <SystemControl.h>

#include <sensors/proximity.h>
#include <motors.h>
#include <leds.h>
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

	// init proximity sensors
	proximity_start();
	// init the motors
	motors_init();
	control_motor_start();

	uint8_t actual_cell = 0;

	/* Infinite loop. */
	while (1) {

		if(get_selector() == POS_SEL_0){					//	selector = 0 --> lwf
			chThdSleepMilliseconds(1500);
			move(LWF_ALGORITHM, actual_cell);
		}else if(get_selector() == POS_SEL_1){				//	selector = 1 --> pledge
			chThdSleepMilliseconds(1500);
			move(PLEDGE_ALGORITHM, actual_cell);
		}else{
			reset_orientation();
			scan_maze_cell(&actual_cell);
		}

		chThdYield();
	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
	chSysHalt("Stack smashing detected");
}
