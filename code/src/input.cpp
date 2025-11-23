
#include <cmath>
#include <algorithm>

#include "debug_utils.h"
#include "input.h"

using namespace std;
using namespace nlohmann;

namespace tsptwm
{
void from_json(const json& j, TimeWindow& tw)
{
	tw.release = j[0];
	tw.deadline = j[1];
}

void to_json(json& j, TimeWindow tw)
{
	j = vector<json>();
	j.push_back(tw.release);
	j.push_back(tw.deadline);
}
   
void to_json(json& j, const Input& input)
{
    j["vertex_count"] = input.vertex_count;
	j["start_depot"] = input.origin;
	j["end_depot"] = input.destination;
	j["horizon"] = input.horizon;
    j["travel_time"] = input.time;
    j["time_windows"] = input.time_window;
}

void from_json(const json& j, Input& input)
{
    input.vertex_count = j["vertex_count"];
    for(auto v = 0ul; v < input.vertex_count; ++v) {
        input.vertices.push_back(v);
        input.vertex_set.set(v);
    }
	input.origin = j["start_depot"];
	input.destination = j["end_depot"];
    input.time_window = vector<TimeWindow>(j["time_windows"]);
    input.horizon = input.time_window[input.origin].deadline;        
    input.time = vector<vector<Time>>(j["travel_times"]);
}

} // namespace tsptwm
