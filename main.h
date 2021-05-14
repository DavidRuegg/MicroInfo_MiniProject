#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <camera/dcmi_camera.h>
#include <msgbus/messagebus.h>
#include <parameter/parameter.h>

// Selector define
#define POS_SEL_0	0
#define POS_SEL_1	1
#define POS_SEL_2	2
#define POS_SEL_3	3
#define POS_SEL_4	4

// LEDs define
#define LED_OFF		0
#define LED_ON		1
#define RGB_MAX		100
#define RGB_MIN		0
#define TOGGLE_LED 	42

// ExitStatus define
#define SEARCHING 0
#define FOUND 1
#define BLOCKED -1

/*** CELL DEFINE ***/
// Bit define
#define WALL_FRONT_BIT	0
#define WALL_RIGHT_BIT 	1
#define WALL_BACK_BIT	2
#define WALL_LEFT_BIT	3
#define BLUE_BIT		4
#define GREEN_BIT		5
#define RED_BIT			6
#define WALL_B 			0x0F
#define COLOR_B 		0x70
#define NO_WALL			0
// Wall EPuck reference define
#define WALL_FRONT_B 	0x01
#define WALL_RIGHT_B 	0x02
#define WALL_BACK_B 	0x04
#define WALL_LEFT_B		0x08
// Color define
#define BLUE_B 			0x10
#define GREEN_B 		0x20
#define CYAN_B 			0x30
#define RED_B 			0x40
#define MAGENTA_B 		0x50
#define YELLOW_B 		0x60
#define WHITE_B 		0x70

// Thread define
#define SLEEP_MODE		1
#define AWAKE_MODE		0

/*** Structure ***/
typedef struct thd_metadata_s{
    uint8_t Sleep;
    thread_reference_t ThdReference;
} thd_metadata_t;

/*** EXTERN VARIABLES ***/
//Robot wide IPC bus
extern messagebus_t bus;
extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
