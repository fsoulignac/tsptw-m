
#include "label.h"

using namespace std;

namespace tsptwm
{    

Route Label::GetRoute(Time departure) const
{
    GraphPath path;
    for(auto it = this; it != nullptr; it = it->prev)
        path.push_back(it->last);
    reverse(path.begin(), path.end());
    return Route(departure, departure+travel_time, path);
}
    
}
