
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
    const PairOperation& op
)
{
    bool res = false;
    for(auto i = 1ul; i < instance.VertexCount()-1; ++i)
    for(auto j = 1ul; j < instance.VertexCount()-1; ++j)
    {
        Route r = *route;
        if(not op(r, i, j)) continue;
        r.arrival = instance.ArrivalTime(r);
        if(r.arrival < route->arrival) 
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
    const vector<PairOperation>& operators
)
{
    bool improve = true;
    while(improve)
    {
        improve = false;
        for(auto& op : operators) 
            improve = LocalImprove(instance, route, op) or improve;
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
    const Route& route
) {
    Route r = route;
    LocalSearch(instance, &r, vector<PairOperation>{SwapOperation, TwoOptOperation, ShiftOperation});
    return r;
}


} // namespace tspd
