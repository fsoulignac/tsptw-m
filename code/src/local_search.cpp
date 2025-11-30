
#include "debug_utils.h"

#include <random>

#include "local_search.h"
#include "time_guardian.h"

using namespace std;

namespace tsptwm {
    
namespace{
    
using PairOperation = std::function<bool(Route&, size_t, size_t)>;

bool LocalImprove(
    const Instance& instance, 
    Route* route, 
    const PairOperation& op,
    bool from_departure
)
{
    bool res = false;
    for(auto i = 1ul; i < instance.VertexCount()-1; ++i)
    for(auto j = 1ul; j < instance.VertexCount()-1; ++j)
    {
        Route r = *route;
        if(not op(r, i, j)) continue;
        r.arrival = instance.ArrivalTime(r, r.departure);
        if(not from_departure) 
        if(r.arrival <= route->arrival)
        {
            auto rr = r;
            reverse(rr.path.begin(), rr.path.end());
            r.departure = instance.Horizon() - instance.ArrivalTime(Direction::Backward, rr, instance.Horizon() - r.arrival);
        }
        if(r.Duration() < route->Duration()) 
        {
            *route = r;
            res = true;
        }
    }

    return res;
} 

void LocalSearch(
    const Instance& instance, 
    Route* route, 
    const vector<PairOperation>& operators,
    bool from_departure
)
{
    bool improve = true;
    while(improve)
    {
        improve = false;
        for(auto& op : operators) 
            improve = LocalImprove(instance, route, op, from_departure) or improve;
    }
} 

bool SwapOperation(Route& r, size_t i, size_t j) {
    if(i >= j) return false;
    swap(r.path[i], r.path[j]); 
    return true;    
}

bool TwoOptOperation(Route& r, size_t i, size_t j) {
    if(i >= j) return false;
    reverse(r.path.begin()+i, r.path.begin() + j);
    return true;    
}

bool ShiftOperation(Route& r, size_t i, size_t j) {
    if(i == j or r.path[j-1] == r.path[j+1] or r.path[j] == r.path[i-1] or r.path[j] == r.path[i]) return false;
    auto tmp = r.path[j];
    r.path.erase(r.path.begin() + j);
    r.path.insert(r.path.begin() + i, tmp);
    return true;
}
}

Route LocalSearch(
    const Instance& instance, 
    const Route& route,
    bool from_departure
) {
    Route r = route;
    LocalSearch(instance, &r, vector<PairOperation>{SwapOperation, TwoOptOperation, ShiftOperation}, from_departure);
    return r;
}


} // namespace tspd
