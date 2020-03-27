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


const char* TAG="opencv_tests";
const int REPEAT = 3;       // number of times to repeat the function call for the average
const int NB_IMAGES = 3;    // number of images with different resolutions for the tests
const String images_res[] = {"160x120", "320x240", "640x480"};


extern "C" {
void app_main(void);
}


Mat read_image_specific_res(const String &fileName)
{
    Mat img = imread(fileName, IMREAD_GRAYSCALE);

    if(img.empty()) {
        ESP_LOGW(TAG, "cannot read the image: %s", fileName.c_str());
        return img;
    }
    ESP_LOGI(TAG, "Image read of %dx%dx%d, with %d pixel depth", img.rows, img.cols, img.channels(), img.depth());
    return img;
}

void test_thresholds(const Mat &src)
{
    Mat dst;

    // apply thresholds
    BENCHMARK_MS("Binary threshold", REPEAT, threshold, src, dst, 128, 255, THRESH_BINARY);
    BENCHMARK_MS("Triangle threshold", REPEAT, threshold, src, dst, 0, 255, THRESH_BINARY | THRESH_TRIANGLE);
    BENCHMARK_MS("OTSU threshold", REPEAT, threshold, src, dst, 0, 255, THRESH_BINARY | THRESH_OTSU);
    BENCHMARK_MS("To zero threshold", REPEAT, threshold, src, dst, 128, 255, THRESH_TOZERO);
}

void test_blurring(const Mat &src)
{
    Mat dst;

    // apply blurs
    BENCHMARK_MS("GaussianBlur9x9", REPEAT, GaussianBlur, src, dst, Size(9, 9), 0, 0, BORDER_DEFAULT);
    BENCHMARK_MS("medianBlur9x9", REPEAT, medianBlur, src, dst, 9);
    BENCHMARK_MS("bilateralFilter", REPEAT, bilateralFilter, src, dst, 9, 18, 5, BORDER_DEFAULT);
}

void test_morphology_transform(const Mat &src)
{
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
    Mat dst;

    BENCHMARK_MS("linear resize", REPEAT, resize, src, dst, Size(), 0.75, 0.75, INTER_LINEAR);
    BENCHMARK_MS("cubic resize", REPEAT, resize, src, dst, Size(), 0.75, 0.75, INTER_CUBIC);
    BENCHMARK_MS("pyrUp", REPEAT, pyrUp, src, dst, Size(), BORDER_DEFAULT);
    // FIXME: pyrDown uses a lot of stack and then causes the next function to crash with a LoadProhibited
    //BENCHMARK_MS("pyrDown", REPEAT, pyrDown, src, dst, Size(), BORDER_DEFAULT);
}

void test_edge_detect(const Mat &src)
{
    Mat dst;

    BENCHMARK_MS("Sobel", REPEAT, Sobel, src, dst, 2, 2, 1, 3, 1, 0, BORDER_DEFAULT);
    //BENCHMARK_MS("Canny", REPEAT, Canny, src, dst, 8, 24, 3, false);     // FIXME: can't deduce template parameter 'F'
    Canny(src, dst, 8, 24, 3, false);   // cache warm up
    auto start = chrono::system_clock::now();
    Canny(src, dst, 8, 24, 3, false);
    auto duration = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now() - start);
    std::cout << "Canny" << ": " << duration.count() << " [" << "ms" << "]" << std::endl;
}

void test_hough(const Mat &src)
{
    Mat dst, blurred, edge;

    blur(src, blurred, Size(3,3));
    Canny(blurred, edge, 10, 30, 3, false);

    // hough
    vector<Vec2f> lines;
    BENCHMARK_MS("HoughLines", REPEAT, HoughLines, edge, lines, 1, CV_PI/180, 100, 0, 0, 0, CV_PI);

    // probabilistic hough
    vector<Vec4i> linesP;
    BENCHMARK_MS("HoughLineP", REPEAT, HoughLinesP, edge, linesP, 1, CV_PI/180, 100, 80, 0);
}


void app_main(void)
{
    ESP_LOGI(TAG, "Starting main");
    disp_mem_infos();

    /* SPIFFS init */
    init_spiffs();

    /* Read the images for the tests */
    Mat src = read_image_specific_res("/spiffs/"+images_res[0]+".png");
    Mat src2 = read_image_specific_res("/spiffs/"+images_res[1]+".png");
    Mat src3 = read_image_specific_res("/spiffs/"+images_res[2]+".png");
    vector<Mat> matrices;
    matrices.push_back(src);
    matrices.push_back(src2);
    matrices.push_back(src3);

    // TODO: rename measure.hpp in benchmark.hpp and add printHeader method
    // TODO: make an array of function and call them from a single loop

    /* Conversions and thresholds tests */
    printf("============================== Thresholding tests ==============================\n");
    for(int i = 0; i < NB_IMAGES; i++) {
        printf("Image %s -------------------------\n", images_res[i].c_str());
        test_thresholds(matrices[i]);
    }

    /* Blurring tests */
    printf("================================ Blurring tests ================================\n");
    for(int i = 0; i < NB_IMAGES; i++) {
        printf("Image %s -------------------------\n", images_res[i].c_str());
        test_blurring(matrices[i]);
    }

    /* Morphology transformations */
    printf("======================= Morphology transformations tests =======================\n");
    for(int i = 0; i < NB_IMAGES; i++) {
        printf("Image %s -------------------------\n", images_res[i].c_str());
        test_morphology_transform(matrices[i]);
    }

    /* Edge detection */
    printf("================================ Resizing tests ================================\n");
    for(int i = 0; i < NB_IMAGES; i++) {
        printf("Image %s -------------------------\n", images_res[i].c_str());
        test_resize(matrices[i]);
    }

    /* Image resizing */
    printf("============================= Edge detection tests =============================\n");
    for(int i = 0; i < NB_IMAGES; i++) {
        printf("Image %s -------------------------\n", images_res[i].c_str());
        test_edge_detect(matrices[i]);
    }

    /* Hough transform */
    printf("============================= Hough transform tests ============================\n");
    for(int i = 0; i < NB_IMAGES; i++) {
        printf("Image %s -------------------------\n", images_res[i].c_str());
        test_hough(matrices[i]);
    }

    ESP_LOGI(TAG, "End of main");
}