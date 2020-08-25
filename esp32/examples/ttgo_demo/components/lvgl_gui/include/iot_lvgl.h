#ifndef _COM_GUI_LVGL_H
#define _COM_GUI_LVGL_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"

#include "lv_conf.h"
#include "lvgl.h"


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize LittlevGL GUI 
 */
void lvgl_init();

#ifdef __cplusplus
}
#endif


#endif /* _COM_GUI_LVGL_H */