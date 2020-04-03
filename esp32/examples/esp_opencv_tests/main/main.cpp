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
#include <pthread.h>
#include <thread>
#include <tuple>


#include "system.hpp"
#include "report.hpp"

using namespace cv;
using namespace std;

const char* TAG="opencv_tests";

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


int64_t BM_binThresh(const Mat& src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 128, 255, THRESH_BINARY);
}

int64_t BM_triangleThresh(const Mat& src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 0, 255, THRESH_BINARY | THRESH_TRIANGLE);
}

int64_t BM_OTSUThresh(const Mat& src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 0, 255, THRESH_OTSU | THRESH_BINARY);
}

int64_t BM_toZeroThresh(const Mat& src) {
    Mat dst;
    return BENCHMARK(threshold, src, dst, 128, 255, THRESH_TOZERO);
}

int64_t BM_GaussianBlur3x3(const Mat& src) {
    Mat dst;
    return BENCHMARK(GaussianBlur, src, dst, Size(3, 3), 0, 0, BORDER_DEFAULT);
}

int64_t BM_medianBlur3x3(const Mat& src) {
    Mat dst;
    return BENCHMARK(medianBlur, src, dst, 3);
}

int64_t BM_bilateralFilter(const Mat& src) {
    Mat dst;
    return BENCHMARK(bilateralFilter, src, dst, 5, 18, 5, BORDER_DEFAULT);
}

int64_t BM_erode(const Mat& src) {
    Mat dst;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(4, 4));
    return BENCHMARK(erode, src, dst, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
}

int64_t BM_dilate(const Mat& src) {
    Mat dst;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(4, 4));
    return BENCHMARK(dilate, src, dst, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
}

int64_t BM_open(const Mat& src) {
    Mat dst;
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(4, 4));
    return BENCHMARK(morphologyEx, src, dst, MORPH_OPEN, element, Point(-1,-1), 1, BORDER_CONSTANT, morphologyDefaultBorderValue());
}

int64_t BM_resizeLinear(const Mat& src) {
    Mat dst;
    return BENCHMARK(resize, src, dst, Size(), 0.75, 0.75, INTER_LINEAR);
}

int64_t BM_resizeCubic(const Mat& src) {
    Mat dst;
    return BENCHMARK(resize, src, dst, Size(), 0.75, 0.75, INTER_CUBIC);
}
int64_t BM_Sobel(const Mat& src) {
    Mat dst;
    return BENCHMARK(Sobel, src, dst, 2, 2, 1, 3, 1, 0, BORDER_DEFAULT);
}
int64_t BM_Canny(const Mat& src) {
    Mat dst;
    Canny(src, dst, 50, 200, 3, false);   // cache warm up
    auto start = chrono::system_clock::now();
    Canny(src, dst, 50, 200, 3, false);
    auto duration = chrono::duration_cast<chrono::microseconds >(chrono::system_clock::now() - start);
    return duration.count();
}
int64_t BM_HoughLines(const Mat& src) {
    Mat dst, blurred, edge;
    blur(src, blurred, Size(3,3));
    Canny(blurred, edge, 50, 200, 3, false);
    vector<Vec2f> lines;
    return BENCHMARK(HoughLines, edge, lines, 1, CV_PI/180, 100, 0, 0, 0, CV_PI);
}

int64_t BM_HoughLinesP(const Mat& src) {
    Mat dst, blurred, edge;
    blur(src, blurred, Size(3,3));
    Canny(blurred, edge, 50, 200, 3, false);
    vector<Vec4i> linesP;
    return BENCHMARK(HoughLinesP, edge, linesP, 1, CV_PI/180, 100, 80, 0);
}

void app_main(void)
{
    // Needed for opencv linkage with pthread to work
    pthread_cond_t cond_test = PTHREAD_COND_INITIALIZER;
    pthread_cond_init(&cond_test, nullptr);

    ESP_LOGI(TAG, "Starting main");
    disp_mem_infos();

    /* SPIFFS init */
    init_spiffs();

    /* Read the images for the tests */
    const vector<string> images_res = {"160x120", "320x240", "640x480", "1024x768"};
    imagesList testImages;
    for(const String &res : images_res) {
        testImages.push_back(tuple<string, const Mat&>(res, read_image_specific_res("/spiffs/"+res+".png")));
    }

    Report report("BUILD_TYPE=Release", testImages);

    TestGroup &thresholds = report.addGroup("Threshold");
    thresholds.addTestCase(TestCase("binaryThreshold",  BM_binThresh));
    thresholds.addTestCase(TestCase("triangleThreshold",  BM_triangleThresh));
    thresholds.addTestCase(TestCase("OTSUThreshold", BM_OTSUThresh));
    thresholds.addTestCase(TestCase("toZeroThreshold", BM_toZeroThresh));

    TestGroup &blurrings = report.addGroup("Blurring");
    blurrings.addTestCase(TestCase("GaussianBlur 3x3 kernel", BM_GaussianBlur3x3));
    blurrings.addTestCase(TestCase("medianBlur 3x3 kernel", BM_medianBlur3x3));
    blurrings.addTestCase(TestCase("bilateralFilter diameter=5", BM_bilateralFilter));

    TestGroup &morph = report.addGroup("Morphological tranforms");
    morph.addTestCase(TestCase("erode 5x5 kernel", BM_erode));
    morph.addTestCase(TestCase("dilate 5x5 kernel", BM_dilate));
    morph.addTestCase(TestCase("open 5x5 kernel", BM_open));

    TestGroup &resize = report.addGroup("Resize image");
    resize.addTestCase(TestCase("resize linear interpolation", BM_resizeLinear));
    resize.addTestCase(TestCase("resize cubic interpolation", BM_resizeCubic));

    TestGroup &edgeDetect = report.addGroup("Edge detection");
    edgeDetect.addTestCase(TestCase("Sobel", BM_Sobel));
    edgeDetect.addTestCase(TestCase("Canny", BM_Canny));

    TestGroup &houghTransform = report.addGroup("Hough tranformations");
    houghTransform.addTestCase(TestCase("HoughLines", BM_HoughLines));
    houghTransform.addTestCase(TestCase("HoughLines probabilistic", BM_HoughLinesP));

    report.startBenchmark();

    string summary = report.getSummary();
    cout << summary << endl;

    ESP_LOGI(TAG, "End of main");
}