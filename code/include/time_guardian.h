/*
 * @author Francisco Soulignac
 * @brief A singleton to keep track of time limits
 */
 
#ifndef TSPTWM_TIME_GUARDIAN_H
#define TSPTWM_TIME_GUARDIAN_H

#include <exception>
#include "stopwatch.h"

namespace tsptwm {

/**
 * @class TimeLimitExceeded
 * @author Francisco
 * @brief Exception to control the time limit.
 */
struct TimeLimitExceeded : public std::exception {
    inline const char * what () const noexcept{
        return "Time Limit Exceeded";
    }
};

/**
 * @class TimeGuardian
 * @author Francisco
 * @brief A simple class to keep a global stopwatch (@see Stopwatch.h) that allows us to fail in a
 * centralized manner if the time limit is reached.
 */
class TimeGuardian {
public:
    TimeGuardian() = default;
    
    /**
     * @brief Sets the time limit for this guardian.
     * @param dur duration after it can fail
     */
    void SetTimeLimit(const Duration& dur);
    
    /**
     * @brief Raises a TimeLimitExceeded exception if its time limit was reached
     */
    void FailIfTimeLimit() const;
    
    /**
     * @brief Returns the time the guardian has been living
     * @return the age of the guardian.
     */
    Duration Now() const;
    
    /**
     * @brief Returns the time limit
     */
    Duration GetTimeLimit() const;
private:
    Stopwatch rolex;   //keeps track of time; it never stops
    Duration time_limit{0};   //time limit; initially 0, thus it fails if no time limit is set and Fail... is called
};

/** Singleton instance to unify time. */
extern TimeGuardian time_guardian;

} //namespace tsptwm
#endif //TSPTWM_TIME_GUARDIAN_H
