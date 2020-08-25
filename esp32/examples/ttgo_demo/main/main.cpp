#undef EPS      // specreg.h defines EPS which interfere with opencv
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#define EPS 192

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>
#include <iostream>

#include "system.h"
#include "app_screen.h"
#include "app_camera.h"

#include "iot_lvgl.h"

using namespace cv;

extern "C" {
void app_main(void);
}


#define TAG "main"

extern CEspLcd *tft;

/**
 * Task doing the demo: Getting image from camera, processing it with opencv depending on the displayMode and
 * displaying it on the lcd
 */
void demo_task(void *arg) {
    ESP_LOGI(TAG, "Starting demo_task");

    // Display memory infos
    disp_infos();

    while(true) {

        camera_fb_t *fb = esp_camera_fb_get();

        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
        } else {
            TFT_jpg_image(CENTER, 0, 0, -1, NULL, fb->buf, fb->len);
            esp_camera_fb_return(fb);
            fb = NULL;
        }

#if 0   // TODO: next step..
        Mat inputImage(fb->height, fb->width, CV_8UC2, fb->buf);      // rgb565 is 2 channels of 8-bit unsigned
        Mat outputImage;


        if(currentDisplayMode == DisplayMode::RGB) {
        }
        else if(currentDisplayMode == DisplayMode::GRAYSCALE) {
            cvtColor(inputImage, inputImage, COLOR_BGR5652GRAY);
        }
        else if(currentDisplayMode == DisplayMode::BINARIZED) {
            cvtColor(inputImage, inputImage, COLOR_BGR5652GRAY);
            threshold(inputImage, inputImage, 128, 255, THRESH_BINARY);
        }
        else if(currentDisplayMode == DisplayMode::EDGES) {
            cvtColor(inputImage, inputImage, COLOR_BGR5652GRAY);
            // Reduce noise with a kernel 3x3
            blur(inputImage, inputImage, Size(3, 3));
            /** Apply the canny edges detector with:
             * - low threshold = 50
             * - high threshold = 4x low
             * - sobel kernel size = 3x3
             */
            int lowThresh = 40;
            int kernSize = 3;
            Canny(inputImage, inputImage, lowThresh, 4 * lowThresh, kernSize);
        }
        else {
            ESP_LOGE(TAG, "Wrong display mode: %d", (int)currentDisplayMode);
        }

        // Diplay image on LCD
        setImage(inputImage);

#endif

        wait_msec(25);
    }
}

void app_main()
{
    ESP_LOGI(TAG, "Starting main");

    /* initializations */
    app_camera_init();
    lvgl_init();

    /* Display memory infos */
    disp_infos();

    /* Start the tasks */
    xTaskCreatePinnedToCore(demo_task, "demo", 1024 * 9, nullptr, 24, nullptr, 0);
}