/**
 * @file	DataProcess.c
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	16.05.2021
 *
 * @brief	Algorithms to solve a maze (Pledge, Left wall follower).
 * 			Some actions depending on wall and color detections.
 */

#include <leds.h>

#include <main.h>
#include <SystemControl.h>


/*** STATIC VARIABLES ***/
/* Keeps track of the orientation of the e-puck all the time
 * used in Pledge and lwf (useless in lwf if used without Pledge)
 */
static int8_t EPuckOrientation = 0;


/*** PUBLIC FUNCTIONS ***/

void reset_orientation(void){
	EPuckOrientation = 0;
}

int16_t left_wall_follower(uint8_t Cell_Ref_EPuck){	// lwf algorithm
	if(!(Cell_Ref_EPuck & WALL_LEFT_B)){				// turn left possible --> turn left
		EPuckOrientation--;
		return LEFT_TURN;
	}else if(!(Cell_Ref_EPuck & WALL_FRONT_B)){			// ...impossible --> move forward if possible
		return MOVE_FORWARD;
	}else if(!(Cell_Ref_EPuck & WALL_RIGHT_B)){			// ...impossible --> turn right if possible
		EPuckOrientation++;
		return RIGHT_TURN;
	}else{												// ...impossible --> turn backward
		EPuckOrientation += 2;
		return BACKWARD_TURN;
	}
}

int16_t pledge_algorithm(uint8_t Cell_Ref_EPuck){
	if(EPuckOrientation == 0){					// condition for Pledge algorithm
		if(!(Cell_Ref_EPuck & WALL_FRONT_B)){			// move forward possible --> move forward
			return MOVE_FORWARD;
		}else if(!(Cell_Ref_EPuck & WALL_RIGHT_B)){		// ...impossible --> turn right if possible
			EPuckOrientation++;
			return RIGHT_TURN;
		}else if (!(Cell_Ref_EPuck & WALL_BACK_B)){		// ...impossible --> turn backward if possible
			EPuckOrientation += 2;
			return BACKWARD_TURN;
		}else{											// ...impossible --> turn left
			EPuckOrientation--;
			return LEFT_TURN;
		}
	}else{										// if not Pledge algorithm --> left wall follower algorithm
		return left_wall_follower(Cell_Ref_EPuck);
	}
}

void set_wall_leds(uint8_t Cell_Ref_EPuck){
	set_led(LED1, ((Cell_Ref_EPuck & WALL_FRONT_B)>>WALL_FRONT_BIT));
	set_led(LED3, ((Cell_Ref_EPuck & WALL_RIGHT_B)>>WALL_RIGHT_BIT));
	set_led(LED5, ((Cell_Ref_EPuck & WALL_BACK_B)>>WALL_BACK_BIT));
	set_led(LED7, ((Cell_Ref_EPuck & WALL_LEFT_B)>>WALL_LEFT_BIT));
}

void set_floor_leds(uint8_t Cell_Ref_EPuck){
	set_rgb_led(LED4,
			RGB_MAX*((Cell_Ref_EPuck & RED_B)>>RED_BIT),
			RGB_MAX*((Cell_Ref_EPuck & GREEN_B)>>GREEN_BIT),
			RGB_MAX*((Cell_Ref_EPuck & BLUE_B)>>BLUE_BIT));
	set_rgb_led(LED6,
			RGB_MAX*((Cell_Ref_EPuck & RED_B)>>RED_BIT),
			RGB_MAX*((Cell_Ref_EPuck & GREEN_B)>>GREEN_BIT),
			RGB_MAX*((Cell_Ref_EPuck & BLUE_B)>>BLUE_BIT));
}

void floor_color_action(uint8_t Cell_Ref_EPuck){
	switch (Cell_Ref_EPuck & COLOR_B) {
	case RED_B:		// Action: increase speed
		correction_nominal_speed(+CORRECTION_SPEED);
		break;
	case GREEN_B:	// Action: decrease speed
		correction_nominal_speed(-CORRECTION_SPEED);
		break;
	case BLUE_B:	// Action: spin on itself
		turn(ONE_TURN);
		break;
	default:
		break;
	}
}

void check_exit(uint8_t Cell_Ref_EPuck, int8_t* EPuckStatus){
	int8_t NewStatus = SEARCHING;						// Default:	SEARCHING

	// Checks status
	if((Cell_Ref_EPuck & WALL_B) == WALL_B){			// 4 walls:	BLOCKED
		NewStatus = BLOCKED;
	}else if ((Cell_Ref_EPuck & WALL_B) == NO_WALL){	// No wall:	FOUND
		NewStatus = FOUND;
	}

	// Clears LEDs if status has changed
	if(NewStatus != *EPuckStatus){
		set_body_led(LED_OFF);
		set_front_led(LED_OFF);
		*EPuckStatus = NewStatus;
	}
}

/*** END PUBLIC FUNCTIONS ***/
