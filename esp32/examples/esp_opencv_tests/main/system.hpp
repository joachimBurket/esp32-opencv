//
// Created by joachim on 24.03.20.
//

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

// Initialize the SPIFFS
esp_err_t init_spiffs();

// Display cpu and memory info
void disp_mem_infos();

// Holds the task for a given amount of msec
void wait_msec(uint16_t v);

// Holds the task for a full second
void wait_sec(uint16_t v);

#endif //SYSTEM_HPP
