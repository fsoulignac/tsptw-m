/*
 * @author Francisco Soulignac
 * @brief A simple stopwatch
 * 
 * Code adapted from different sources
 */

#ifndef TSPTWM_STOPWATCH_H
#define TSPTWM_STOPWATCH_H

#include "types.h"

namespace tsptwm {
    
class Stopwatch {
public:
    /** Kind of clock used */
    using clock = std::chrono::high_resolution_clock;

    /** Creates a stopwatch counting time from now */
    Stopwatch();

    /** Resets the start time to now */
    void Reset();

    /** Returns the elapsed time since its starting counting */
    Duration Elapsed() const;

private:
    std::chrono::time_point<clock> start_time;
};

}   //namespace tsptwm

/**
 * Formatter for the stopwatch (we just use seconds)
 */
template<>
struct std::formatter<tsptwm::Stopwatch> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::Stopwatch& w, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}s", tsptwm::to_seconds(w.Elapsed()));
    }
};


#endif //TSPTWM_STOPWATCH_H