/*
 * @author Francisco Soulignac
 * @brief Simple, yet effective, utilities for debugging which can be disabled for release.
 */


#define DEBUG

#ifdef DEBUG
#include <format>
#include "print.h"
extern int debug_level_random_xvzh;
#define PRINT -1
#define ASSERTION 0
#define INFO 1
#define VERBOSE 2
#define TRACE 3
#define OVERKILL 4
#endif //DEBUG

#ifdef DEBUG
    /**
     * @brief This macro sets the debug level to x, where x is any integer number.
     *
     * From this point and until the level is updated again, every debugging instruction of level at most x
     * will be executed
     */
    #define SET_DEBUG_LEVEL(x) do {debug_level_random_xvzh = x;} while(false)
#else
    /**
     * @brief This macro has no effects and is probably removed by the compiler
     */
    #define SET_DEBUG_LEVEL(x) do {} while(false)
#endif //DEBUG

#ifdef DEBUG 
    /** @brief Outputs fmt with parameters ... into the console log if the debug level is at least i.
     *
     * Here fmt and ... is anything that follows the std::format function
     */
    #define DEBUG_ELEM(i, fmt, ...) do {if(debug_level_random_xvzh >= i) clog << std::format(fmt, ##__VA_ARGS__) << endl;} while(false)
#else
    /**
     * @brief This macro has no effects and is probably removed by the compiler
     */
    #define DEBUG_ELEM(i, fmt, ...) do {} while(false)
#endif //DEBUG

#ifdef DEBUG 
    /** @brief Executes the code in x if the debug level is at least i.
     *
     * Here x is any code that compiles in C++
     */
    #define DEBUG_CODE(i, x) do {if(debug_level_random_xvzh >= i) {x}} while(false)
#else
    /**
     * @brief This macro has no effects and is probably removed by the compiler
     */
    #define DEBUG_CODE(i, x) do {} while(false)        
#endif

/** @brief Ends the program with message fmt ... if b if false
 *
 * Here fmt and ... is anything that follows the std::format function
 */
#define ASSERT(i, b, fmt, ...) DEBUG_CODE(i, if(not (b)) {DEBUG_ELEM(i, "ASSERTION FAILED! {}", std::format(fmt, ##__VA_ARGS__)); throw std::runtime_error("ASSERTION FAILED!"); exit(127);})
