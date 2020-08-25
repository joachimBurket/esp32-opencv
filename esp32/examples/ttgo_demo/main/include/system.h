#ifndef __SYSTEM_H
#define __SYSTEM_H


// Display memory info
void disp_infos();

// Holds the task for a given amount of msec
void wait_msec(uint16_t v);

// Holds the task for a full second
void wait_sec(uint16_t v);

#endif
