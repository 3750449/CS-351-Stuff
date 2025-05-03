
#ifndef __STREAM_TIMER_H__
#define __STREAM_TIMER_H__

#include <chrono>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <string>
#include <sstream>

//----------------------------------------------------------------------------
//
//  Unit strings for std::ratio types
//

namespace {

using seconds = std::ratio<1,1>;
using milli   = std::milli;
using micro   = std::micro;
using nano    = std::nano;

template <typename ratio = seconds> struct ratio_traits
    { static constexpr const char* units_string = " s";};

template <> struct ratio_traits<std::nano>
    { static constexpr const char* units_string = " ns"; };

template <> struct ratio_traits<std::micro>
    { static constexpr const char* units_string = " μs"; };

template <> struct ratio_traits<std::milli>
    { static constexpr const char* units_string = " ms"; };

} // namespace

//----------------------------------------------------------------------------
//
//  ScopeTimer
//

template<typename Interval = std::nano>
struct ScopeTimer {

    using Clock = std::chrono::high_resolution_clock;
    using Time = std::chrono::time_point<Clock>;

    std::string msg;
    Time        start = Clock::now();

    const char* units = ratio_traits<Interval>::units_string;

    ScopeTimer(const char* m = "") : msg(m) {
        if (!(msg.empty() || msg.back() == ' ')) { msg += " "; }
    }

    void elapsed() const {
        auto end = Clock::now();
        std::chrono::duration<double, Interval> elapsed{ end - start };

        // Set the output precision for values
        auto previous = std::cerr.precision();
        std::cerr.precision(15);
        std::cerr << msg << elapsed.count() << units << std::endl;
        std::cerr.precision(previous);
    }

    ~ScopeTimer() {
        elapsed();
    }
};

// A C-preprocessor macro that will set the output message to
//   the function's name (__func__)  containing the block being
//   timed, and the file's line number (__LINE__).
//
#define TIME_BLOCK() ScopeTimer timer_{ __func__, __LINE__ }

#endif // __STREAM_TIMER_H__
