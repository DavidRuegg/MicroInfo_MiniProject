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
extern thd_metadata_t ControlMotor_MetaData;
extern thd_metadata_t GetProximity_MetaData;
extern thd_metadata_t CaptureImage_MetaData;


/*** INTERNAL FUNCTIONS ***/

void make_thread_sleep(thd_metadata_t* ThdMetaData){
	ThdMetaData->Sleep = SLEEP_MODE;
}

void make_thread_wakeup(thd_metadata_t* ThdMetaData){
	if(ThdMetaData->Sleep){
		chSysLock();
		chThdResumeS(&(ThdMetaData->ThdReference), AWAKE_MODE);
		chSysUnlock();
	}
}

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
		case POS_SEL_0:		// Selector = 0: maze solving with left wall follower algorithm.
			// Clears all LEDs
			set_body_led(0);
			set_front_led(0);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&ControlMotor_MetaData);
			make_thread_wakeup(&GetProximity_MetaData);
			make_thread_wakeup(&CaptureImage_MetaData);

			// Sleeps to be able to remove hands
			chThdSleepMilliseconds(1500);

			while(get_selector() == POS_SEL_0){
				// Updates the EPuckCell with the most recent one
				EPuckCell = get_actual_cell();

				// LEDs + Color action
				set_wall_leds(EPuckCell);
				set_floor_leds(EPuckCell);
				floor_color_action(EPuckCell);

				/* Updates ExitStatus and searches for an exit
				 *		SEARCHING: 	left wall follower algorithm.
				 *		FOUND: 		blink body LED green.
				 *		BLOCKED: 	blink front LED red.
				 */
				check_exit(EPuckCell, &ExitStatus);
				switch (ExitStatus) {
				case SEARCHING:
					move(left_wall_follower(EPuckCell));
					chBSemWait(&motor_ready_sem);
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
			}
			break;
		case POS_SEL_1:		// Selector = 1: maze solving with Pledge algorithm.
			// Resets orientation so that Pledge algorithm is usable without a total reset
			reset_orientation();

			// Clears all LEDs
			set_body_led(0);
			set_front_led(0);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&ControlMotor_MetaData);
			make_thread_wakeup(&GetProximity_MetaData);
			make_thread_wakeup(&CaptureImage_MetaData);

			// Sleeps to be able to remove hands
			chThdSleepMilliseconds(1500);

			while(get_selector() == POS_SEL_1){
				// Updates the EPuckCell with the most recent one
				EPuckCell = get_actual_cell();

				// LEDs + Color action
				set_wall_leds(EPuckCell);
				set_floor_leds(EPuckCell);
				floor_color_action(EPuckCell);

				/* Updates ExitStatus and searches for an exit
				 *		SEARCHING: 	left wall follower algorithm.
				 *		FOUND: 		blink body LED green.
				 *		BLOCKED: 	blink front LED red.
				 */
				check_exit(EPuckCell, &ExitStatus);
				switch (ExitStatus) {
				case SEARCHING:
					move(pledge_algorithm(EPuckCell));
					chBSemWait(&motor_ready_sem);
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
			}
			break;
		case POS_SEL_2:		// Selector = 2: demonstration walls detection.
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);
			make_thread_sleep(&CaptureImage_MetaData);

			// Clear all LEDs
			set_body_led(0);
			set_front_led(0);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&GetProximity_MetaData);

			while(get_selector() == POS_SEL_2){
				// Set wall LEDs
				EPuckCell = get_actual_cell();
				set_wall_leds(EPuckCell);
				chThdYield();
			}
			break;
		case POS_SEL_3:		// Selector = 3: demonstration colors detection.
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);
			make_thread_sleep(&GetProximity_MetaData);

			// Clear all LEDs
			set_body_led(0);
			set_front_led(0);
			clear_leds();

			// Wakes necessary threads up
			make_thread_wakeup(&CaptureImage_MetaData);

			while(get_selector() == POS_SEL_3){
				// Set floor LEDs
				EPuckCell = get_actual_cell();
				set_floor_leds(EPuckCell);
				chThdYield();
			}
			break;
		case POS_SEL_4:		// Selector = 4: walls detection and color detection.
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);

			// Clear body + front LEDs
			set_body_led(0);
			set_front_led(0);

			// Wakes necessary threads up
			make_thread_wakeup(&GetProximity_MetaData);
			make_thread_wakeup(&CaptureImage_MetaData);

			while(get_selector() == POS_SEL_4){
				// Set wall + floor LEDs
				EPuckCell = get_actual_cell();
				set_wall_leds(EPuckCell);
				set_floor_leds(EPuckCell);
				chThdYield();
			}
			break;
		default: 			// Default: send own threads to sleep
			// Sends unnecessary thread to sleep
			make_thread_sleep(&ControlMotor_MetaData);
			make_thread_sleep(&GetProximity_MetaData);
			make_thread_sleep(&CaptureImage_MetaData);

			// Clears all LEDs
			set_body_led(0);
			set_front_led(0);
			clear_leds();
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
