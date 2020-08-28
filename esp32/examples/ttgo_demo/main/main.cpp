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
#include <map>

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

static lv_obj_t *lvCameraImage; // Camera image object

void gui_boot_screen() {
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, LV_STATE_DEFAULT, 2);
    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_MAKE(190, 190, 190));
    lv_style_set_border_width(&style, LV_STATE_DEFAULT, 2);
    lv_style_set_border_color(&style, LV_STATE_DEFAULT, LV_COLOR_MAKE(142, 142, 142));

    lv_style_set_pad_top(&style, LV_STATE_DEFAULT, 60);
    lv_style_set_pad_bottom(&style, LV_STATE_DEFAULT, 60);
    lv_style_set_pad_left(&style, LV_STATE_DEFAULT, 60);
    lv_style_set_pad_right(&style, LV_STATE_DEFAULT, 60);

    lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_MAKE(102, 102, 102));
    lv_style_set_text_letter_space(&style, LV_STATE_DEFAULT, 5);
    lv_style_set_text_line_space(&style, LV_STATE_DEFAULT, 20);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(obj, LV_LABEL_PART_MAIN, &style);
    lv_label_set_text(obj, "TTGO\n"
                           "demo!");
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
    wait_msec(3000);
}

void gui_init() {
    // Create screen
    lv_obj_t *scr = lv_obj_create(nullptr, nullptr);
    lv_scr_load(scr);
}

esp_err_t updateCameraImage(const cv::Mat &img) {
    // static variables because they must still be available when lv_task_handler() is called
    static Mat imgCopy;
    static lv_img_dsc_t my_img_dsc;

    if(img.empty()) {
        ESP_LOGW(TAG, "Can't display empty image");
        return ESP_ERR_INVALID_ARG;
    }

    // convert image to bgr565 if needed
    if(img.type() == CV_8UC1) {         // grayscale image
        cvtColor(img, imgCopy, COLOR_GRAY2BGR565, 1);
    }
    else if(img.type() == CV_8UC3) {    // BGR888 image
        cvtColor(img, imgCopy, COLOR_BGR2BGR565, 1);
    }
    else if(img.type() == CV_8UC2) {    // BGR565 image
        img.copyTo(imgCopy);
    }

    my_img_dsc.header.always_zero = 0;
    my_img_dsc.header.w = imgCopy.cols;
    my_img_dsc.header.h = imgCopy.rows;
    my_img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    my_img_dsc.data_size = imgCopy.size().width * imgCopy.size().height;
    my_img_dsc.data = imgCopy.ptr<uchar>(0);

    lv_img_set_src(lvCameraImage, &my_img_dsc);  /* Set the created file as image */
    lv_obj_set_pos(lvCameraImage, -40, 0);

    return ESP_OK;
}

// mode of the demo
enum class DisplayMode : uint8_t {RGB, GRAYSCALE, BINARIZED, EDGES, NUM_OF_MODES};
static DisplayMode currentDisplayMode;

static const std::string displayModeToString(DisplayMode dispMode) {
    const std::map<DisplayMode, const std::string> DisplayModeStrings {
            {DisplayMode::RGB, "RGB"},
            {DisplayMode::GRAYSCALE, "GRAYSCALE"},
            {DisplayMode::BINARIZED, "BINARIZED"},
            {DisplayMode::EDGES, "EDGES"},
    };
    auto it = DisplayModeStrings.find(dispMode);
    return (it == DisplayModeStrings.end()) ? "Out of range" : it->second;
}

/**
 * Task doing the demo: Getting image from camera, processing it with opencv depending on the displayMode and
 * displaying it on the lcd
 */
void demo_task(void *arg) {
    ESP_LOGI(TAG, "Starting demo_task");

    // Display memory infos
    disp_infos();

    tft->setRotation(2);        // rotation needed if camera is on the back of the device
    sensor_t *s = esp_camera_sensor_get();

    // Init camera image Lvgl object
    lvCameraImage = lv_img_create(lv_disp_get_scr_act(nullptr), nullptr);
    lv_obj_move_foreground(lvCameraImage);

    while(true) {
        auto start = esp_timer_get_time();

        camera_fb_t *fb = esp_camera_fb_get();

        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
        } else {
            if(s->pixformat == PIXFORMAT_JPEG) {
                TFT_jpg_image(CENTER, CENTER, 0, -1, NULL, fb->buf, fb->len);
                esp_camera_fb_return(fb);
                fb = NULL;
            }
            else {  // RGB565 pixformat
                Mat inputImage(fb->height, fb->width, CV_8UC2, fb->buf);      // rgb565 is 2 channels of 8-bit unsigned

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

                // display image on lcd
                updateCameraImage(inputImage);
            }
        }

        ESP_LOGI(TAG, "%s mode: around %f fps", displayModeToString(currentDisplayMode).c_str(), 1.0f/((esp_timer_get_time() - start) / 1000000.0f));
    }
}

/**
 * Task changing the current displayMode at regular interval
 */
void timer_task(void *arg) {
    while(true) {
        wait_msec(3000);
        currentDisplayMode = static_cast<DisplayMode>((static_cast<int>(currentDisplayMode) + 1) % static_cast<int>(DisplayMode::NUM_OF_MODES));
    }
}

void app_main()
{
    ESP_LOGI(TAG, "Starting main");

    /* initializations */
    app_camera_init();
    lvgl_init();
    gui_init();

    /* display boot screen */
    gui_boot_screen();

    /* Display memory infos */
    disp_infos();

    ESP_LOGI(TAG, "Display width = %d, height = %d", tft->width(), tft->height());

    /* Start the tasks */
    xTaskCreatePinnedToCore(demo_task, "demo", 1024 * 9, nullptr, 24, nullptr, 0);
    xTaskCreatePinnedToCore(timer_task, "timer", 1024 * 1, nullptr, 24, nullptr, 0);
}