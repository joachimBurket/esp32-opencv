#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <esp_log.h>
#include <string>
#include "sdkconfig.h"
#include <iostream>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp_err.h>
#include <esp_spiffs.h>

using namespace cv;
using namespace std;


static char tag[]="opencv_tests";

extern "C" {
void app_main(void);
}


esp_err_t init_spiffs() {
    ESP_LOGI("spiffs_init", "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 10,
            .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("spiffs_init", "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("spiffs_init", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("spiffs_init", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("spiffs_init", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("spiffs_init", "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}


void test_basics()
{

}

void test_spiffs()
{

}


void app_main(void)
{
    ESP_LOGI(tag, "Starting main");

    /* SPIFFS init */
    init_spiffs();

    /* Matrices initialization tests */
    Mat M1(2,2, CV_8UC3, Scalar(0,0,255));
    cout << "M1 = " << endl << " " << M1 << endl << endl;

    Mat M2(2,2, CV_8UC3, Scalar(0,0,111));
    cout << "M2 = " << endl << " " << M2 << endl << endl;

    Mat eye = Mat::eye(10, 10, CV_32F) * 0.1;
    cout << "eye = " << endl << " " << eye << endl << endl;

    Mat ones = Mat::ones(15, 4, CV_8U)*3;
    cout << "ones = " << endl << " " << ones << endl << endl;

    vector<float> v;
    v.push_back((float)CV_PI);
    v.push_back(2);
    v.push_back(3.01f);
    cout << "floats vector = " << endl << " " << Mat(v) << endl << endl;

    /* Matrices imgproc operations tests */
    uint8_t data[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    Mat M3 = Mat(3, 5, CV_8UC1, data);
    cout << "Gray matrix = " << endl << " " << M3 << endl << endl;

    Mat M4;
    threshold(M3, M4, 7, 255, THRESH_BINARY);
    cout << "Thresholded matrix = " << endl << " " << M4 << endl << endl;

    Mat M5;
    resize(M3, M5, Size(), 0.75, 0.75);
    cout << "Resized matrix = " << endl << " " << M5 << endl << endl;

    /* SPIFFS file reading test */
    FILE* f = fopen("/spiffs/hello.txt", "r");
    if (f == NULL) {
        ESP_LOGE(tag, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);

    ESP_LOGI(tag, "Read from file: '%s'", line);

    /* Images reading/writing tests */
    struct stat st;
    if (stat("/spiffs/jack.png", &st) == 0) {
        ESP_LOGI(tag, "jack exists!");
    }

    Mat src, src_gray, src_bin;
    string imageName = "/spiffs/jack.png";
    src = imread(imageName, IMREAD_COLOR);
    if(src.empty()) {
        ESP_LOGW(tag, "cannot read the image: %s", imageName.c_str());
        return;
    }
    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    threshold(src_gray, src_bin, 128, 255, THRESH_BINARY);
    imwrite("/spiffs/jack_bin.png", src_bin);
    ESP_LOGI(tag, "Binary image written!");

    /* some more complex image operations test */
    Mat detected_edges;

    // Reduce noise with a kernel 3x3
    blur(src_gray, detected_edges, Size(3,3));

    /** Apply the canny edges detector with:
     * - low threshold = 5
     * - high threshold = 3x low
     * - sobel kernel size = 3x3
     */
    int lowThresh = 8;
    int kernSize = 3;
    Canny(detected_edges, detected_edges, lowThresh, 3*lowThresh, kernSize);
    imwrite("/spiffs/jack_canny.png", detected_edges);
    ESP_LOGI(tag, "Canny image written!");
}