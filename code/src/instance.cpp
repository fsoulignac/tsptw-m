
#include <queue>
#include <numeric>

#include "debug_utils.h"
#include "instance.h"

using namespace std;

namespace tsptwm
{
    
Instance::Instance(const Input* the_tsp)
  : tsp(the_tsp)
{
    window = tsp->time_window;

    for(auto d : {Direction::Forward, Direction::Backward})
        unreachable[idx(d)].assign(VertexCount(), {{-1, VertexSet()}});
        
    UpdatePredecessors();
}

Time Instance::ArrivalTime(Direction d, Vertex v, Vertex w, Time t) const
{
    if(t > GetTimeWindow(d, v).deadline) return infty_time;
    t = max(max(t, GetTimeWindow(d, v).release) + TravelTime(d, v, w), GetTimeWindow(d,w).release);
    return t > GetTimeWindow(d, w).deadline ? infty_time : t;
}


Time Instance::ArrivalTime(Direction d, const Route& route, Time t0) const {
    if(route.path.empty()) return t0;
    
    t0 = max(t0, GetTimeWindow(d, route.path[0]).release);
    if(t0 > GetTimeWindow(d, route.path[0]).deadline) return infty_time;
    
    for(auto i = 0ul; i < route.path.size()-1 and t0 < infty_time; ++i)
        t0 = ArrivalTime(d, route.path[i], route.path[i+1], t0);

    return t0;
}


bool Instance::UpdateTimeWindow(Direction d, Vertex v, TimeWindow wdw) {
    DEBUG_ELEM(TRACE, "UpdateTimeWindow(direction: {}, vertex: {}, time: {}). Previous window: {}", d, v, wdw, window[v]);    
    if(d == Direction::Backward) {
        wdw = TimeWindow{Horizon() - wdw.deadline, Horizon() - wdw.release};
        DEBUG_ELEM(TRACE, "UpdateTimeWindow(direction: {}, vertex: {}, time: {})", op(d), v, wdw);
    }
    bool changed = wdw.release > window[v].release or wdw.deadline < window[v].deadline;
    window[v].release = max(window[v].release, wdw.release);
    window[v].deadline = min(window[v].deadline, wdw.deadline);
    DEBUG_ELEM(TRACE, "\tNew fwd window: {}",  window[v]);
    return changed;
}

void Instance::UpdatePredecessors() {
    DEBUG_ELEM(INFO, "Updating predecessors and preprocessing time windows");
    auto dijkstra = [&](Direction dir, Vertex from) -> vector<Time> {
        
        auto best = vector<Time>(VertexCount(), infty_time);
        best[from] = GetTimeWindow(dir, from).release;
        
        priority_queue<tuple<Time, Vertex>> q; // negated arrival time + vertex
        q.emplace(-best[from], from);
        
        VertexSet visited;
        while (!q.empty()) {
            auto [na, v] = q.top(); auto a = -na;
            q.pop();
            if(visited.test(v)) continue;
            visited.set(v);
            
            for(auto w : Vertices()) 
            if(not visited.test(w)) {
                auto tw = GetTimeWindow(dir, w);
                if(auto aw = max(a + TravelTime(dir, v, w), tw.release); aw < best[w])
                if(aw <= tw.deadline)
                {
                    q.emplace(-aw, w);
                    best[w] = aw;
                }
            }
        }
        return best;
    };

    for(bool changed = true; changed;) {
        DEBUG_ELEM(INFO, "New iteration of preprocessing");
        changed = false;
        for(auto dir : {Direction::Forward, Direction::Backward}) {
            vector<vector<Time>> LDT(VertexCount(), vector<Time>(VertexCount(), -1));
            for(auto v : Vertices())
            {
                auto best_back = dijkstra(op(dir), v);
                for(auto w = 0ul; w < VertexCount(); ++w)
                if(w != Destination(dir))
                    LDT[w][v] = best_back[w] == infty_time ? -1 : Horizon() - best_back[w];
            }
                    
            vector<vector<Time>> EAT(VertexCount(), vector<Time>(VertexCount(), infty_time));
            for(auto v : Vertices())
            {
                auto best = dijkstra(dir, v);
                for(auto w = 0ul; w < VertexCount(); ++w)
                if(w != Origin(dir))
                    EAT[v][w] = best[w];
            }
            
            // v puede preceder a w si, para todo z, es factible alguna de las siguientes
            // 1) z -> v -> w => EAT(z, v) <= LDT(v,w)
            // 2) v -> w -> z => EAT(v, w) <= LDT(w,z)
            // 3) v -> z -> w => EAT(v, z) <= LDT(z, w)
            // En caso que ninguna sea factible actualizamos LDT(v, w) = 0
            for(auto v : Vertices())
            for(auto w : Vertices()) 
            if(w != v)
            if(LDT[v][w] >= 0)
            for(auto z : Vertices()) 
            if(z != v)
            if(z != w)
            if(EAT[z][v] > LDT[v][w])
            if(EAT[v][w] > LDT[w][z])
            if(EAT[v][z] > LDT[z][w])
            {
                LDT[v][w] = -1;
                EAT[v][w] = infty_time;
            }    
            
            
            for(auto v : Vertices())
            {
                vector<pair<Time, Vertex>> sorted_ldt;
                for(auto w = 0ul; w < VertexCount(); ++w)
                    sorted_ldt.emplace_back(LDT[v][w], w);
                sort(sorted_ldt.begin(), sorted_ldt.end());

                auto& U = unreachable[idx(dir)][v];
                U = {{-1, VertexSet()}};
                for(auto [t, w] : sorted_ldt) 
                if(w != v)    
                {                
                    if(t >= U.back().first) {
                        U.emplace_back(t+1, U.back().second);
                    }
                    U.back().second.set(w);
                }   
                DEBUG_ELEM(TRACE, "Unreachable of {} in direction {}: {}", v, dir, U);
            }

            // Preprocess time windows
            for(auto v : Vertices())
            {
                Time deadline = v != Destination(dir) ? 0 : infty_time;
                Time release = v != Origin(dir) ? infty_time : 0;
                for(auto w : Vertices()) {
                    if(LDT[w][v] >= GetTimeWindow(dir, w).release)
                        deadline = max(deadline, ArrivalTime(dir, w, v, LDT[w][v]));
                    if(EAT[w][v] <= GetTimeWindow(dir, v).deadline)
                        release = min(release, EAT[w][v]);
                }
                changed = UpdateTimeWindow(dir, v, {release, deadline}) or changed;
                DEBUG_ELEM(TRACE, "Time window of {} in direction {}: {}", v, dir, GetTimeWindow(dir, v));
            }
        }
    }
}

const VertexSet& Instance::Unreachable(Direction dir, Vertex v, Time t) const
{
    ASSERT(ASSERTION, t >= 0, "Unreachable: invalid time");
    auto u_idx = unreachable[idx(dir)][v].size();
    while(t < unreachable[idx(dir)][v][u_idx-1].first) u_idx -= 1;
    ASSERT(ASSERTION, u_idx > 0, "Unreachable: invalid index");
    return unreachable[idx(dir)][v][u_idx-1].second;
}

} // namespace tsptwm
