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


static char TAG[]="hello_opencv";

extern "C" {
void app_main(void);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting main");

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
}