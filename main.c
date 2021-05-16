/**
 * @file	main.c
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	16.05.2021
 *
 * @brief	Solve a maze made of fixed cells dimensions.
 * 			Based on e-puck robot and cells of size 115x115mm with right angle.
 * 			Handle wall detection and color detection.
 * 			Manage threads and actions depending on selector position.
 */

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
extern binary_semaphore_t MotorReady_sem;
extern thd_metadata_t ControlMotor_MetaData;
extern thd_metadata_t GetProximity_MetaData;
extern thd_metadata_t CaptureImage_MetaData;


/*** INTERNAL FUNCTIONS ***/

/**
 * @brief	Send a thread to sleep. Can be called from another thread.
 * 			 The thread in question has to check his Sleep variable
 * 			 before entering in sleep mode.
 *
 * @param	ThdMetaData		Variable containing thread metadata
 */
void make_thread_sleep(thd_metadata_t* ThdMetaData){
	ThdMetaData->Sleep = SLEEP_MODE;
}


/**
 * @brief	Wake a thread up.
 *
 * @param	ThdMetaData		Variable containing thread metadata
 */
void make_thread_wakeup(thd_metadata_t* ThdMetaData){
	if(ThdMetaData->Sleep){
		chSysLock();
		chThdResumeS(&(ThdMetaData->ThdReference), AWAKE_MODE);
		chSysUnlock();
	}
}

/*** MAIN ***/
int main(void){
	/*** INTERNAL VARIABLES ***/
	uint8_t EPuckCell = 0;
	int8_t ExitStatus = SEARCHING;

	/*** INITIALIZATION ***/
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
	proximity_acquisition_start();
	color_acquisition_start();

	// sleeps to get everything correctly initialized
	chThdSleepMilliseconds(2000);

	/*** INFINITE LOOP ***/
	while(1){
		/*** E-PUCK MODES OF OPERATION
		 *		Selector = 0: maze solving with left wall follower algorithm.
		 *		Selector = 1: maze solving with Pledge algorithm.
		 *		Selector = 2: demonstration walls detection.
		 *		Selector = 3: demonstration colors detection.
		 *		Selector = 4: walls detection and color detection.
		 *		Default		: send own threads to sleep
		 ***/
		switch(get_selector()){
		case POS_SEL_0:	// Selector = 0: maze solving with left wall follower algorithm.
			// Clears all LEDs
			set_body_led(LED_OFF);
			set_front_led(LED_OFF);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&ControlMotor_MetaData);
			make_thread_wakeup(&GetProximity_MetaData);
			make_thread_wakeup(&CaptureImage_MetaData);

			do{
				// Updates the EPuckCell with the most recent one
				EPuckCell = get_actual_cell();

				// Sets LEDs
				set_wall_leds(EPuckCell);
				set_floor_leds(EPuckCell);

				/* Updates ExitStatus and searches for an exit
				 *		SEARCHING: 	left wall follower algorithm.
				 *		FOUND: 		blink body LED green.
				 *		BLOCKED: 	blink front LED red.
				 */
				check_exit(EPuckCell, &ExitStatus);
				switch (ExitStatus) {
				case SEARCHING:
					floor_color_action(EPuckCell);
					go_next_cell(left_wall_follower(EPuckCell));
					chBSemWait(&MotorReady_sem);
					break;
				case FOUND:
					set_body_led(TOGGLE_LED);
					chThdSleepMilliseconds(500);
					break;
				case BLOCKED:
					set_front_led(TOGGLE_LED);
					chThdSleepMilliseconds(500);
				default:
					break;
				}
			}while(get_selector() == POS_SEL_0);
			break;

		case POS_SEL_1:	// Selector = 1: maze solving with Pledge algorithm.
			// Resets orientation so that Pledge algorithm is usable without a total reset
			reset_orientation();

			// Clears all LEDs
			set_body_led(LED_OFF);
			set_front_led(LED_OFF);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&ControlMotor_MetaData);
			make_thread_wakeup(&GetProximity_MetaData);
			make_thread_wakeup(&CaptureImage_MetaData);

			do{
				// Updates the EPuckCell with the most recent one
				EPuckCell = get_actual_cell();

				// Sets LEDs
				set_wall_leds(EPuckCell);
				set_floor_leds(EPuckCell);

				/* Updates ExitStatus and searches for an exit
				 *		SEARCHING: 	left wall follower algorithm.
				 *		FOUND: 		blink body LED green.
				 *		BLOCKED: 	blink front LED red.
				 */
				check_exit(EPuckCell, &ExitStatus);
				switch (ExitStatus) {
				case SEARCHING:
					floor_color_action(EPuckCell);
					go_next_cell(pledge_algorithm(EPuckCell));
					chBSemWait(&MotorReady_sem);
					break;
				case FOUND:
					set_body_led(TOGGLE_LED);
					chThdSleepMilliseconds(500);
					break;
				case BLOCKED:
					set_front_led(TOGGLE_LED);
					chThdSleepMilliseconds(500);
				default:
					break;
				}
			}while(get_selector() == POS_SEL_1);
			break;

		case POS_SEL_2:	// Selector = 2: demonstration walls detection.
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);
			make_thread_sleep(&CaptureImage_MetaData);

			// Clear all LEDs
			set_body_led(LED_OFF);
			set_front_led(LED_OFF);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&GetProximity_MetaData);

			do{
				// Set wall LEDs
				EPuckCell = get_actual_cell();
				set_wall_leds(EPuckCell);
				chThdYield();
			}while(get_selector() == POS_SEL_2);
			break;

		case POS_SEL_3:	// Selector = 3: demonstration colors detection.
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);
			make_thread_sleep(&GetProximity_MetaData);

			// Clear all LEDs
			set_body_led(LED_OFF);
			set_front_led(LED_OFF);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&CaptureImage_MetaData);

			do{
				// Set floor LEDs
				EPuckCell = get_actual_cell();
				set_floor_leds(EPuckCell);
				chThdYield();
			}while(get_selector() == POS_SEL_3);
			break;

		case POS_SEL_4:	// Selector = 4: walls detection and color detection.
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);

			// Clear body + front LEDs
			set_body_led(LED_OFF);
			set_front_led(LED_OFF);

			// Wakes necessary threads up
			make_thread_wakeup(&GetProximity_MetaData);
			make_thread_wakeup(&CaptureImage_MetaData);

			do{
				// Set wall + floor LEDs
				EPuckCell = get_actual_cell();
				set_wall_leds(EPuckCell);
				set_floor_leds(EPuckCell);
				chThdYield();
			}while(get_selector() == POS_SEL_4);
			break;

		default: 		// Default: send own threads to sleep
			// Only once
			if(	!ControlMotor_MetaData.Sleep ||
					!GetProximity_MetaData.Sleep ||
					!CaptureImage_MetaData.Sleep ){

				// Sends unnecessary thread to sleep
				make_thread_sleep(&ControlMotor_MetaData);
				make_thread_sleep(&GetProximity_MetaData);
				make_thread_sleep(&CaptureImage_MetaData);

				// Clears all LEDs
				set_body_led(LED_OFF);
				set_front_led(LED_OFF);
				clear_leds();
			}
			break;
		}

		// Sleeps to be able to remove hands before a switch of selector
		chThdSleepMilliseconds(1500);
	}
	/*** END INFINITE LOOP ***/
}
/*** END MAIN ***/

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
	chSysHalt("Stack smashing detected");
}
/*** END INTERNAL FUNCTIONS ***/
