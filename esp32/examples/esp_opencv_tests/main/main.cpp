#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <esp_log.h>
#include <esp_err.h>
#include <esp_task_wdt.h>
#include <esp_timer.h>

#include <string>
#include <chrono>
#include <iostream>
#include <sys/unistd.h>



#include "system.hpp"
#include "measure.hpp"

using namespace cv;
using namespace std;


char* TAG="opencv_tests";
const String imageFileName = "/spiffs/jack.png";
const int REPEAT = 3;       // number of times to repeat the function call for the average

extern "C" {
void app_main(void);
}


/**
 * Function that tests the basics of OpenCV:
 * - Matrices creation
 * - Arithmetic operations
 * -
 */
void test_basics()
{
    ESP_LOGI(TAG, "Doing basic matrices test");

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

    uint8_t data[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    Mat M3 = Mat(3, 5, CV_8UC1, data);
    cout << "Gray matrix = " << endl << " " << M3 << endl << endl;
}

/**
 * Function that reads and write on the SPI Flash File System
 */
void test_spiffs()
{
    ESP_LOGI(TAG, "Doing SPIFFS read/write tests");

    /* SPIFFS file reading test */
    FILE* f = fopen("/spiffs/hello.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);

    printf("Read from file: '%s'\n", line);

    /* Images reading/writing tests */
    Mat src;
    src = imread(imageFileName, IMREAD_COLOR);
    if(src.empty()) {
        ESP_LOGW(TAG, "cannot read the image: %s", imageFileName.c_str());
        return;
    }

    imwrite("/spiffs/dst.png", src);
    ESP_LOGI(TAG, "Image written!");
}

void test_thresholds(const Mat &src)
{
    ESP_LOGI(TAG, "==================== Thresholding tests ====================");

    Mat gray, dst;

    // convert to grayscale
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // apply thresholds
    BENCHMARK_MS("Binary threshold", REPEAT, threshold, gray, dst, 128, 255, THRESH_BINARY);
    BENCHMARK_MS("Triangle threshold", REPEAT, threshold, gray, dst, 0, 255, THRESH_BINARY | THRESH_TRIANGLE);
    BENCHMARK_MS("OTSU threshold", REPEAT, threshold, gray, dst, 0, 255, THRESH_BINARY | THRESH_OTSU);
    BENCHMARK_MS("To zero threshold", REPEAT, threshold, gray, dst, 128, 255, THRESH_TOZERO);
}

void test_blurring(const Mat &src)
{
    ESP_LOGI(TAG, "==================== Blurring tests ====================");

    Mat dst;

    // apply blurs
    BENCHMARK_MS("GaussianBlur", REPEAT, GaussianBlur, src, dst, Size(9, 9), 0, 0, BORDER_DEFAULT);
    BENCHMARK_MS("medianBlur", REPEAT, medianBlur, src, dst, 9);
    BENCHMARK_MS("bilateralFilter", REPEAT, bilateralFilter, src, dst, 9, 18, 5, BORDER_DEFAULT);
}

void test_morphology_transform(const Mat &src)
{
    ESP_LOGI(TAG, "=========== Morphology transformations tests ===========");

    Mat dst;

    // create a kernel for the transformation
    Mat element = getStructuringElement(MORPH_RECT, Size(9, 9), Point(4, 4));

    // apply transformations
    BENCHMARK_MS("erode", REPEAT, erode, src, dst, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
    BENCHMARK_MS("dilate", REPEAT, dilate, src, dst, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
    BENCHMARK_MS("open", REPEAT, morphologyEx, src, dst, MORPH_OPEN, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
}

void test_resize(const Mat &src)
{
    ESP_LOGI(TAG, "==================== Resizing tests ====================");

    Mat dst;

    BENCHMARK_MS("linear resize", REPEAT, resize, src, dst, Size(), 0.75, 0.75, INTER_LINEAR);
    BENCHMARK_MS("cubic resize", REPEAT, resize, src, dst, Size(), 0.75, 0.75, INTER_CUBIC);
    BENCHMARK_MS("pyrUp", REPEAT, pyrUp, src, dst, Size(), BORDER_DEFAULT);
    // FIXME: pyrDown uses a lot of stack and then causes the next function to crash with a LoadProhibited
    //BENCHMARK_MS("pyrDown", REPEAT, pyrDown, src, dst, Size(), BORDER_DEFAULT);
}

void test_edge_detect(const Mat &src)
{
    ESP_LOGI(TAG, "================= Edge detection tests =================");

    Mat dst, gray;

    // convert to grayscale
    cvtColor(src, gray, COLOR_BGR2GRAY);

    BENCHMARK_MS("Sobel", REPEAT, Sobel, gray, dst, 2, 2, 1, 3, 1, 0, BORDER_DEFAULT);
    //BENCHMARK_MS("Canny", REPEAT, Canny, gray, dst, 8, 24, 3, false);     // FIXME: can't deduce template parameter 'F'
    auto start = chrono::system_clock::now();
    Canny(gray, dst, 8, 24, 3, false);
    auto duration = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now() - start);
    std::cout << "Time taken by " << "Canny" << ": " << duration.count() << " [" << "ms" << "]" << std::endl;
}

void test_hough(const Mat &src)
{
    ESP_LOGI(TAG, "================= Hough transform tests =================");

    Mat dst, gray, edge;

    // Grayscale and edge detection
    cvtColor(src, gray, COLOR_BGR2GRAY);
    Canny(gray, edge, 8, 24, 3, false);

    // hough
    vector<Vec2f> lines;
    BENCHMARK_MS("HoughLines", REPEAT, HoughLines, edge, lines, 1, CV_PI/180, 200, 0, 0, 0, CV_PI);
    cout << lines.size() << " lines were detected" << endl;

    // probabilistic hough
    vector<Vec4i> linesP;
    BENCHMARK_MS("HoughLineP", REPEAT, HoughLinesP, edge, linesP, 1, CV_PI/180, 200, 30, 10);
    cout << linesP.size() << " lines were detected" << endl;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting main");
    disp_mem_infos();

    /* SPIFFS init */
    init_spiffs();

    /* Basics Matrices tests */
    test_basics();
    
    /* Test the spiffs read/write */
    test_spiffs();
    disp_mem_infos();

    /* Read an image for the next tests */
    Mat src = imread(imageFileName, IMREAD_COLOR);
    if(src.empty()) {
        ESP_LOGW(TAG, "cannot read the image: %s", imageFileName.c_str());
        return;
    }
    ESP_LOGI(TAG, "Image read of %dx%dx%d, with %d pixel depth", src.rows, src.cols, src.channels(), src.depth());

    /* Conversions and thresholds tests */
    test_thresholds(src);
    disp_mem_infos();

    /* Blurring tests */
    test_blurring(src);
    disp_mem_infos();

    /* Morphology transformations */
    test_morphology_transform(src);
    disp_mem_infos();

    /* Edge detection */
    test_edge_detect(src);
    disp_mem_infos();

    /* Image resizing */
    test_resize(src);
    disp_mem_infos();

    wait_msec(500);

    /* Hough transform */
    test_hough(src);
    disp_mem_infos();

    ESP_LOGI(TAG, "End of main");
}