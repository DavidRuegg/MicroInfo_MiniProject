/**
 * @file	DataAcquisition.c
 *
 * @author	David 	RUEGG
 * @author	Thibaut	STOLTZ
 *
 * @date	14.05.2021
 *
 * @brief	Thread to acquire proximity data based on IR sensors.
 * 			Threads to acquire colors detection based on CMOS camera.
 * 			Static variable and getter to save environment.
 */

#include <camera/po8030.h>
#include <camera/dcmi_camera.h>
#include <sensors/proximity.h>
#include <leds.h>

#include <main.h>
#include <DataAcquisition.h>


/*** GLOBAL VARIABLES ***/
thd_metadata_t CaptureImage_MetaData = {.Sleep = 0, .ThdReference = NULL};
thd_metadata_t GetProximity_MetaData = {.Sleep = 0, .ThdReference = NULL};


/*** STATIC VARIABLES ***/
static BSEMAPHORE_DECL(ImageReady_sem, FALSE);
/* Variable ActualCell is continuously updated by threads: GetProximity and ProcessImage,
 *  according to the environment.
 * Bits 0 to 3 are set to 1 if the corresponding
 *  wall is around the e-puck.
 * 		Bit 0 --> front wall
 * 		Bit 1 --> right wall
 *		Bit 2 --> back wall
 *		Bit 3 --> left wall
 * Bits 4 to 6 are set to 1 according to the color of the floor.
 *		Bit 4 --> blue
 *		Bit 5 --> green
 *		Bit 6 --> red
 */
static uint8_t ActualCell;


/*** INTERNAL FUNCTIONS ***/

/**
 * @brief	Thread which retrieves continuously proximity data.
 * 			Sets corresponding walls to static variable ActualCell.
 */
static THD_WORKING_AREA(waGetProximity, 64);
static THD_FUNCTION(GetProximity, arg){
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	/* variables used when two sensors are used for the same wall, as an "or" condition
	 *  example condition: (IR1 || IR8) --> wall detected
	 */
	uint8_t Check_IR18 = 0;
	uint8_t Check_IR45 = 0;

	systime_t Time;

	/*** INFINITE LOOP ***/
	while(1){
		// Enters sleep mode if asked by another thread.
		if(GetProximity_MetaData.Sleep){
			chSysLock();
			GetProximity_MetaData.Sleep = chThdSuspendS(&GetProximity_MetaData.ThdReference);
			chSysUnlock();
		}

		Time = chVTGetSystemTime();

		/*** SCAN FOR WALLS ***
		 * Walls are saved on bits 0 to 3
		 * 	IR1 and IR8 --> front wall
		 * 	IR3 		--> right wall
		 * 	IR4 and IR5 --> back wall
		 * 	IR6			--> left wall
		 */
		for(uint8_t i=0 ; i<PROXIMITY_NB_CHANNELS ; i++){
			if((i == IR2) || (i == IR7)){					// no use of IR2 and IR7 sensors
				continue;
			}
			else if(get_prox(i) > PROXIMITY_THRESHOLD){		// wall detected --> sets bit to 1
				switch (i) {
				case IR1:
				case IR8:
					ActualCell |= WALL_FRONT_B;
					Check_IR18 = 1;
					break;
				case IR3:
					ActualCell |= WALL_RIGHT_B;
					break;
				case IR4:
				case IR5:
					ActualCell |= WALL_BACK_B;
					Check_IR45 = 1;
					break;
				case IR6:
					ActualCell |= WALL_LEFT_B;
					break;
				default:
					break;
				}
			}else{											// no wall detected --> sets bit to 0
				switch (i) {
				case IR1:
				case IR8:
					if(!Check_IR18){
						ActualCell &= ~WALL_FRONT_B;
					}
					Check_IR18 = 0;
					break;
				case IR3:
					ActualCell &= ~WALL_RIGHT_B;
					break;
				case IR4:
				case IR5:
					if(!Check_IR45){
						ActualCell &= ~WALL_BACK_B;
					}
					Check_IR45 = 0;
					break;
				case IR6:
					ActualCell &= ~WALL_LEFT_B;
					break;
				default:
					break;
				}

			}
		}
		// 20 Hz cycle
		chThdSleepUntilWindowed(Time, Time + MS2ST(50));
	}
	/*** END INFINITE LOOP ***/
}

/**
 * @brief	Thread which configures and captures images.
 * 			Signals semaphore ImageReady_sem when an image has been captured.
 */
static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg){
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	/*** PO8030 CONFIGURATION ***/
	// Image configuration: format --> RGB565, origin --> (220,240), size --> (IMAGE_BUFFER_SIZE, 2)
	po8030_advanced_config(FORMAT_RGB565, 220, 240, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	// White balance disabled in order to identify the colors
	po8030_set_awb(0);
	// RGB gain adjusted to the scene
	po8030_set_rgb_gain(0x52, 0x52 , 0x65);		// Office - Sunny
	//po8030_set_rgb_gain(0x55, 0x4F , 0x65);	// Home - Sunny
	//po8030_set_rgb_gain(0x5E, 0x4F , 0x5D);	// Home - Cloudy
	// Contrast adjusted to the scene
	po8030_set_contrast(20);

	/*** DCMI CONFIGURATION ***/
	// Double buffering enabled in order to process image while capturing another
	dcmi_enable_double_buffering();
	// Capture mode set to one shot
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	// Prepares DCMI unit
	dcmi_prepare();

	/*** INFINITE LOOP ***/
	while(1){
		// Enters sleep mode if asked by another thread.
		if(CaptureImage_MetaData.Sleep){
			chSysLock();
			CaptureImage_MetaData.Sleep = chThdSuspendS(&CaptureImage_MetaData.ThdReference);
			chSysUnlock();
		}

		// Starts a capture
		dcmi_capture_start();

		// Waits for the capture to be done
		wait_image_ready();

		// Signals an image has been captured
		chBSemSignal(&ImageReady_sem);
	}
	/*** END INFINITE LOOP ***/
}

/**
 * @brief	Thread which extracts the colors of the image.
 * 			Sets the colors to RGB front LEDs.
 * 			Sets the colors to static variable ActualCell.
 */
static THD_WORKING_AREA(waProcessImage, 32);
static THD_FUNCTION(ProcessImage, arg){
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	/*** INTERNAL VARIABLES ***/

	uint8_t *ImgBuff_ptr = NULL;

	uint32_t RedVal = 0;
	uint32_t GreenVal = 0;
	uint32_t BlueVal = 0;
	uint16_t MaxVal = 0;
	uint8_t Color = 0;

	/*** INFINITE LOOP ***/
	while(1){
		// Waits until an image has been captured
		chBSemWait(&ImageReady_sem);

		// Gets the pointer to the array filled with the last image in RGB565
		ImgBuff_ptr = dcmi_get_last_image_ptr();

		// Resets values of last image
		RedVal = 0;
		GreenVal = 0;
		BlueVal = 0;
		Color = 0;

		// Extracts and adds all pixels values of one line, by color (format RGB565)
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){	// pixels are acquired on two bytes
			RedVal += (ImgBuff_ptr[i] & 0xF8) >> 2;				// red value scaled to green size
			GreenVal += ((ImgBuff_ptr[i] & 0x07) << 3) +			// green value
					((ImgBuff_ptr[i+1] & 0xE0) >> 5);
			BlueVal += (ImgBuff_ptr[i+1] & 0x1F) << 1;			// blue value scaled to green size
		}

		// Checks for the maximum value of RGB
		if(RedVal >= GreenVal && RedVal >= BlueVal){
			MaxVal = RedVal;
		}else if(GreenVal >= BlueVal){
			MaxVal = GreenVal;
		}else{
			MaxVal = BlueVal;
		}

		// Sets scale to percentage of maximum value
		RedVal = (RedVal*RGB_MAX)/MaxVal;
		GreenVal = (GreenVal*RGB_MAX)/MaxVal;
		BlueVal = (BlueVal*RGB_MAX)/MaxVal;

		// Saves the colors to variable Color
		if(RedVal > COLOR_THRESHOLD){
			Color |= RED_B;
		}
		if(GreenVal > COLOR_THRESHOLD){
			Color |= GREEN_B;
		}
		if(BlueVal > COLOR_THRESHOLD){
			Color |= BLUE_B;
		}

		// Transfers colors to static variable ActualCell and erases previous ones
		ActualCell = ((ActualCell & ~COLOR_B) | Color);

		// Sets camera output to RGB front LEDs
		if(!CaptureImage_MetaData.Sleep){
			set_rgb_led(LED2, RedVal, GreenVal, BlueVal);
			set_rgb_led(LED8, RedVal, GreenVal, BlueVal);
		}else{	// only once if thread CaptureImage went to sleep --> switches off the RGB LEDs
			set_rgb_led(LED2, RGB_MIN, RGB_MIN, RGB_MIN);
			set_rgb_led(LED8, RGB_MIN, RGB_MIN, RGB_MIN);
		}
	}
	/*** END INFINITE LOOP ***/
}

/*** END INTERNAL FUNCTIONS ***/

/*** PUBLIC FUNCTIONS ***/

void proximity_acquisition_start(void){
	chThdCreateStatic(waGetProximity, sizeof(waGetProximity), NORMALPRIO, GetProximity, NULL);
}

void color_acquisition_start(void){
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
}

uint8_t get_actual_cell(void){
	return ActualCell;
}

/*** END PUBLIC FUNCTIONS ***/
