
#include "debug_utils.h"
#include "route.h"

using namespace std;
using namespace nlohmann;

namespace tsptwm
{

Route::Route(Time d, Time a, const GraphPath& p)
  : path(p), 
    departure(d),
    arrival(a)
{ }

void to_json(json& j, const Route& r)
{
	j["kd_type"] = "tsptwm_route";
	j["path"] = r.path;
	j["departure"] = r.departure;
    j["arrival"] = r.arrival;
    j["duration"] = r.arrival - r.departure;
}

} 
