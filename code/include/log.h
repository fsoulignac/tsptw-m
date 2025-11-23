/*
 * @author Francisco Soulignac
 * @brief Logs for the methods; compatible with kaleidoscope (parsers included)
 * 
 * Based on a design by Gonzalo Lera-Romero
 */

#ifndef TSPTWM_LOG_H
#define TSPTWM_LOG_H

#include <vector>
#include <format>
#include "json.hpp"
#include "types.h"

namespace tsptwm
{
    
/**
 * @brief Status that can result from executing the optimization algorithms
 */
enum class SolverStatus {
	DidNotStart, /**< The algorithm hasn't started yet */
    TimeLimitExceeded, /**< The time limit was exceeded */
    MemoryLimitExceeded, /**< The memory limit was exceeded */
    Completed /**< The algorithm completed its task succesfully */
};    

/**
 * @class LBFSLog
 * @author Francisco
 * @brief Log of a labeling BFS algorithm
 * 
 * The log records different statistics about the execution of the LBFS algorithm.
 * This log is compatible with the kaleidoscope type tsptwm_lbfs.
 */
struct LBFSLog
{
    Duration time{0}; /**< total time spent by the algorithm */
    size_t enumerated_count = 0; /**< number of labels enumerated and possibly discarded */
    size_t dominated_count = 0; /**< number of labels totally dominated and discarded withing domination */
    size_t undominated_count = 0; /**< number of labels that were not dominated. */
    size_t discarded_count = 0;   /**< number of labels discarded discounting dominance */
    
    LBFSLog() = default;
};

    
/**
 * @class MinimizeMakespanLog
 * @author Francisco
 * @brief Log of the minimize makespan algorithm
 * 
 * The log records different statistics about the execution of the makespan solver.
 * This log is compatible with the kaleidoscope type tsptwm_ms_solver.
 */
struct MakespanSolverLog
{
	Duration time{0}; /**< total time spent solving the problem. */
	SolverStatus status = SolverStatus::DidNotStart; /**< the final status of the execution. */
	Time ub = infty_time; /**< final upper bound obtained by the solver */
    std::vector<LBFSLog> iterations; /**< logs for the iterations of the LBFS method */

	MakespanSolverLog() = default;	
};

/**
 * @class MinimizeDurationLog
 * @author Francisco
 * @brief Log of the minimize duration algorithm
 * 
 * The log records different statistics about the execution of the makespan solver.
 * This log is compatible with the kaleidoscope type tsptwm_solver.
 */
struct DurationSolverLog 
{
	Duration time{0}; /**< total time spent solving the problem. */
    Duration ms_time{0}; /**< total time spent to solve the makespan problem */
    Duration reverse_ms_time{0}; /**< total time spent to solve the makespan problem in the reverse instance */
	SolverStatus status = SolverStatus::DidNotStart; /**< Final status status of the execution. */
	SolverStatus ms_status = SolverStatus::DidNotStart; /**< Final status status of the execution. */
	SolverStatus reverse_ms_status = SolverStatus::DidNotStart; /**< Final status status of the execution. */
	Time ub = infty_time; /**< best solution ever found */
	Time ms_ub = infty_time; /**< best solution found for the makespan problem */
	Time reverse_ms_ub = infty_time; /**< best solution found for the makespan problem */
    Duration preprocess_time{0}; /**< total time for preprocessing the input */
    Duration enum_time{0}; /**< total time for enumeration */
    Duration ms_preprocess_time{0}; /**< total time for preprocessing the input */
    Duration ms_enum_time{0}; /**< total time for enumeration */
    std::vector<MakespanSolverLog> iterations; /**< logs for the iterations of the makespan solver */
    
	DurationSolverLog() = default;
};


/**
 * @brief Serializes the log into the JSON format.
 * @param j JSON object
 * @param r route to serialize
 */
void to_json(nlohmann::json& j, const LBFSLog& log);
void to_json(nlohmann::json& j, const MakespanSolverLog& log);
void to_json(nlohmann::json& j, const DurationSolverLog& log);

} // namespace tsptwm

/** 
 * Formatter of the Solver Status
 */
template<>
struct std::formatter<tsptwm::SolverStatus> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::SolverStatus& status, std::format_context& ctx) const {
        std::string s;
        switch(status) {
            case tsptwm::SolverStatus::DidNotStart: s = "DidNotStart"; break;
            case tsptwm::SolverStatus::TimeLimitExceeded: s = "TimeLimit"; break;
            case tsptwm::SolverStatus::MemoryLimitExceeded: s = "MemoryLimit"; break;
            default: s = "Completed";
        }
        return std::format_to(ctx.out(), "{}", s);
    }
};

/**
 * Formatter for the LBFS log
 */
template<>
struct std::formatter<tsptwm::LBFSLog> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::LBFSLog& log, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", nlohmann::json(log).dump(4));
    }
};

/**
 * Formatter for the MakespanSolver log
 */
template<>
struct std::formatter<tsptwm::MakespanSolverLog> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::MakespanSolverLog& log, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", nlohmann::json(log).dump(4));
    }
};
 
/**
 * Formatter for the DurationSolver log
 */   
template<>
struct std::formatter<tsptwm::DurationSolverLog> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::DurationSolverLog& log, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", nlohmann::json(log).dump(4));
    }
};

#endif //TSPTWM_LOG
