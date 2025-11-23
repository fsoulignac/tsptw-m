
#include "log.h"

using namespace std;
using namespace nlohmann;

namespace tsptwm
{

void to_json(json& j, const LBFSLog& log)
{
	j["kd_type"] = "tsptwm_lbfs"; // ID of the log type.
	j["time"] = to_seconds(log.time);
	j["enumerated_count"] = log.enumerated_count;
	j["dominated_count"] = log.dominated_count;
	j["undominated_count"] = log.undominated_count;
	j["discarded_count"] = log.discarded_count;
}

void to_json(json& j, const MakespanSolverLog& log)
{
	j["kd_type"] = "tsptwm_ms_solver";
	j["time"] = to_seconds(log.time);
	j["status"] = format("{}", log.status);
	j["ub"] = log.ub;
    j["iterations"] = log.iterations;
}

void to_json(json& j, const DurationSolverLog& log)
{
	j["kd_type"] = "tsptwm_solver";
	j["time"] = to_seconds(log.time);
	j["makespan_time"] = to_seconds(log.ms_time);
	j["reverse_makespan_time"] = to_seconds(log.reverse_ms_time);
	j["status"] = format("{}", log.status);
	j["makespan_status"] = format("{}", log.ms_status);
	j["reverse_makespan_status"] = format("{}", log.reverse_ms_status);
	j["ub"] = log.ub;
	j["ms_ub"] = log.ms_ub;
	j["reverse_ms_ub"] = log.reverse_ms_ub;
	j["preprocess_time"] = to_seconds(log.preprocess_time);
	j["enum_time"] = to_seconds(log.enum_time);
	j["ms_preprocess_time"] = to_seconds(log.ms_preprocess_time);
	j["ms_enum_time"] = to_seconds(log.ms_enum_time);
    j["iterations"] = log.iterations;
}

} // namespace tsptwm
