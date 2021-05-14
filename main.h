#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "camera/dcmi_camera.h"
#include "msgbus/messagebus.h"
#include "parameter/parameter.h"

#define POS_SEL_0	0
#define POS_SEL_1	1
#define POS_SEL_2	2
#define POS_SEL_3	3
#define POS_SEL_4	4

#define SLEEP_MODE 1
#define AWAKE_MODE 0

typedef struct thd_metadata_s{
    uint8_t Sleep;
    thread_reference_t ThdReference;
} thd_metadata_t;

/*** Robot wide IPC bus. ***/
extern messagebus_t bus;
extern parameter_namespace_t parameter_root;

#ifdef __cplusplus
}
#endif

#endif
