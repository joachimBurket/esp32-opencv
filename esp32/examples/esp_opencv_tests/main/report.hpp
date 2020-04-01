//
// Helper to benchmark time taken by functions
//
// Code taken from https://github.com/picanumber/bureaucrat/blob/master/time_lapse.h
// and https://github.com/espressif/esp-dsp/blob/master/test/report.inc
//

#ifndef REPORT_HPP
#define REPORT_HPP

#include <opencv2/core.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <string>
#include <algorithm>

#define BENCHMARK(fct, ...) Report::benchmark(fct, __VA_ARGS__)

using functionSignature = std::function<int (const cv::Mat&)>;
using imagesList = std::vector<std::tuple<std::string, cv::Mat> >;

#define fw(what) std::forward<decltype(what)>(what)

                        // ==============
                        // class TestCase
                        // ==============
class TestCase
{
public:
    static constexpr int DEFAULT_ITERATIONS = 3;

    std::string name;
    std::vector<float> resultTime;
    int iterations;
    functionSignature func;

    TestCase(std::string _name, int _iterations, functionSignature _func);
    TestCase(std::string _name, functionSignature _func);

    void executeOn(const imagesList &testImages);
};

                        // ===============
                        // class TestGroup
                        // ===============
class TestGroup
{
public:
    std::string name;
    std::vector<TestCase> tests;

    explicit TestGroup(std::string _name);

    void addTestCase(const TestCase& test);
};

                        // =============
                        // class Summary
                        // =============
class Summary
{
private:
    const int COL1_WIDTH = 50;
    const int COL_VALUES_WIDTH = 15;
    imagesList images;
    std::vector<int> columnsWidth;
    int totalWidth;

    std::string lineSeparator(const char sep);

    std::string alignTextLeft(const std::string &text, int colWidth);

    std::string alignTextRight(const std::string &text, int colWidth);

    std::string alignTextCenter(const std::string &text, int colWidth);

    std::string lineText(const std::vector<std::string>& colTexts);

public:
    Summary(imagesList _images);

    std::string summaryHeader(const std::string &reportName);

    std::string summarySection(const TestGroup& group);

    std::string summaryTest(const TestCase& test);
};


                        // ============
                        // class Report
                        // ============
class Report
/**
 * @ class report
 * @ brief Class to report a benchmark of image processing functions
 */
{
private:
    std::string name;
    std::vector<TestGroup> groups;
    imagesList images;
    Summary summary;

public:
    explicit Report(std::string _name, const imagesList& _images);

    TestGroup& addGroup(const std::string &_name);

    TestGroup& getGroup(const std::string &_name);

    void startBenchmark();

    std::string getSummary();

    /**
     * Function that executes the function 'repeat' times, benchmark the average time taken and logs on the console
     * the time.
     * @tparam F
     * @tparam Args
     * @param unit String representing the time unit (for the log). Can be either 's', 'ms', 'us' or 'ns'
     * @param repeat Number of times to do the benchmark
     * @param func Function to benchmark
     * @param args Arguments of the function 'func'
     */
    template<typename F, typename... Args>
    static int64_t benchmark(int repeat, F&& func, Args&&... args)
    {
        auto avg = duration(func, (args)...);

        for(int i = 0; i < repeat-1; i++) {
            avg += duration(func, (args)...);
        }
        return (avg/repeat).count();
    }

    template<typename F, typename... Args>
    static int64_t benchmark(F&& func, Args&&... args)
    {
        const int defaultIterations = 3;
        return benchmark(defaultIterations, func, (args)...);
    }

    using TimeT = std::chrono::microseconds ;
    using ClockT = std::chrono::system_clock ;

    /**
    * @ fn    duration
    * @ brief Returns the duration (in chrono's type system microseconds) of the elapsed time
    */
    template<typename F, typename... Args>
    static TimeT duration(F&& func, Args&&... args)
    {
        auto start = ClockT::now();

        fw(func)(std::forward<Args>(args)...);

        return std::chrono::duration_cast<TimeT>(ClockT::now() - start);
    }
};

#undef fw

#endif  // REPORT_HPP
