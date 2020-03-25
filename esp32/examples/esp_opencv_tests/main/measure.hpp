//
// Helper to measure time taken by functions
//
// Code taken from https://github.com/picanumber/bureaucrat/blob/master/time_lapse.h
//

#ifndef MEASURE_HPP
#define MEASURE_HPP

#include <chrono>
#include <iostream>
#include <string>

/*
 * @brief Macro to simplify the benchmark() function usage.
 *          - more concise
 *          - already fill the unit type
 *
 * @param name (String) representing the name of the function (for the log)
 * @param repeat (Int) number of times to call the function for the time average
 * @param fct (F type) function to call
 * @param ... (Args type) arguments of the function to call
 *
 * Usage examples:
 * - without macro:
 * measure<chrono::milliseconds>::benchmark("ms", "medianBlur", repeat, medianBlur, src, dst, 9);
 * - with macro:
 * BENCHMARK_MS("medianBlur", repeat, medianBlur, src, dst, 9);
 */
#define BENCHMARK_NS(name, repeat, fct, ...) measure<chrono::nanoseconds>::benchmark("ns", name, repeat, fct, __VA_ARGS__)
#define BENCHMARK_US(name, repeat, fct, ...) measure<chrono::microseconds>::benchmark("us", name, repeat, fct, __VA_ARGS__)
#define BENCHMARK_MS(name, repeat, fct, ...) measure<chrono::milliseconds>::benchmark("ms", name, repeat, fct, __VA_ARGS__)
#define BENCHMARK_S(name, repeat, fct, ...) measure<chrono::seconds>::benchmark("s", name, repeat, fct, __VA_ARGS__)

#define fw(what) std::forward<decltype(what)>(what)

/**
	* @ class measure
	* @ brief Class to measure the execution time of a callable
	*/
template <
        typename TimeT = std::chrono::milliseconds, class ClockT = std::chrono::system_clock
>
struct measure
{
    /**
    * @ fn    execution
    * @ brief Returns the quantity (count) of the elapsed time as TimeT units
    */
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args)
    {
        auto start = ClockT::now();

        fw(func)(std::forward<Args>(args)...);

        auto duration = std::chrono::duration_cast<TimeT>(ClockT::now() - start);

        return duration.count();
    }

    /**
     * Function that executes the function 'repeat' times, measure the average time taken and logs on the console
     * the time.
     * @tparam F
     * @tparam Args
     * @param unit String representing the time unit (for the log). Can be either 's', 'ms', 'us' or 'ns'
     * @param repeat Number of times to do the measure
     * @param func Function to benchmark
     * @param args Arguments of the function 'func'
     */
    template<typename F, typename... Args>
    static void benchmark(const std::string &unit, const std::string &name, int repeat, F&& func, Args&&... args)
    {
        auto avg = duration(func, (args)...);

        for(int i = 0; i < repeat-1; i++) {
            avg += duration(func, (args)...);
        }

        std::cout << "Time taken by " << name << ": " << (avg / repeat).count() << " [" << unit << "]" << std::endl;
    }

    /**
    * @ fn    duration
    * @ brief Returns the duration (in chrono's type system) of the elapsed time
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


#endif  // MEASURE_HPP
