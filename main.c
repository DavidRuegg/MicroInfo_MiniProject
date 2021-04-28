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


	/*** INTERNAL VARIABLES ***/
	uint8_t actual_cell = 0;

	chThdSleepMilliseconds(2000);
	scan_maze_cell(&actual_cell);

	/* Infinite loop. */
	while (1) {



		chBSemWait(&motor_ready_sem);
		scan_maze_cell(&actual_cell);
		if(!(actual_cell & WALL_B)){
			chThdSleepMilliseconds(500);
			set_body_led(42);
		}else if(!(actual_cell & WALL_LEFT_B)){
			turn(LEFT_TURN);
			chBSemWait(&motor_ready_sem);
			go_next_cell(ONE_CELL);
		}else if(!(actual_cell & WALL_FRONT_B)){
			go_next_cell(ONE_CELL);
		}else if(!(actual_cell & WALL_RIGHT_B)){
			turn(RIGHT_TURN);
			chBSemWait(&motor_ready_sem);
			go_next_cell(ONE_CELL);
		}else{
			turn(BACKWARD_TURN);
			chBSemWait(&motor_ready_sem);
			go_next_cell(ONE_CELL);
		}

		// Wait 1s
		//chThdSleepMilliseconds(2000);
		chThdYield();

	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
	chSysHalt("Stack smashing detected");
}
