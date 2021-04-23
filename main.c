#include "hal.h"
#include "ch.h"
#include "memory_protection.h"

#include <main.h>
#include <DataAcquisition.h>
#include <sensors/proximity.h>
#include <motors.h>
#include <leds.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

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
	//init the motors
	motors_init();

	/*** INTERNAL VARIABLES ***/
	uint8_t actual_cell = 0;

	/* Infinite loop. */
	while (1) {

		set_led(NUM_LED,0);
		scan_maze_cell(&actual_cell);

		//waits 100 us
		chThdSleepMilliseconds(100);
	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
	chSysHalt("Stack smashing detected");
}
