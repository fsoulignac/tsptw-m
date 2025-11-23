
#include <climits>
#include <queue>
#include <ext/pb_ds/assoc_container.hpp>

#include "debug_utils.h"
#include "solver.h"
#include "stopwatch.h"
#include "label.h"
#include "time_guardian.h"
#include "local_search.h"

using namespace std;
using namespace __gnu_pbds;

template<class K, class V>
using hash_table = gp_hash_table<K, V, hash<K>>;


namespace tsptwm
{    

namespace {
    bool is_subset(const VertexSet& x, const VertexSet& y) {return (x & ~y).none();}
}

pair<Route, MakespanSolverLog> SolveMakespan(const Instance& instance, const Route& init_route) {

    DEBUG_ELEM(INFO, "Solver::InformedSearch()");
    MakespanSolverLog log;

    // Initial upper bound
    auto ub = init_route;
    if(ub.path.empty()) {
        std::iota(ub.path.begin(), ub.path.end(), 0);
        ub.departure = 0;
        ub.arrival = instance.Horizon();
    }

    // Output to console
    clog << format("{:>12}\t{:<30}{:>12}{:>12}\n", "Time", "Process", "Duration", "BKS");
    auto stream_info = [&](const std::string& process, const Duration& time) {
        clog << format("{:>12.2f}\t{:<30}{:>12.2f}{:>12}\n", to_seconds(time_guardian.Now()), process, to_seconds(time), ub.arrival);
    };
    

    Stopwatch rolex;
    bool solved = false;
    try {
        while(not solved) {                
            Route r;
            LBFSLog bfs_log;
            try {
                tie(r, bfs_log) = LBFS(instance, Direction::Backward, instance.Horizon() - ub.arrival, instance.Horizon());
                stream_info(format("DFSEnumerate {}", ub.arrival), bfs_log.time);
            } catch(std::bad_alloc& e) {
                log.status = SolverStatus::MemoryLimitExceeded;
                stream_info("DFSEnumerate (MLE)", rolex.Elapsed());
                break;
            } catch(TimeLimitExceeded& tle) {
                log.status = SolverStatus::TimeLimitExceeded;
                stream_info("DFSEnumerate (TLE)", rolex.Elapsed());
                break;
            }          
        
            if(not r.path.empty()) {
                ub = r;
                DEBUG_ELEM(INFO, "DFSExact: Improved elementary route {}", r);
            } else {
                DEBUG_ELEM(INFO, "DFS Enumeration completed");
                solved = true;
            } 
        }
    } catch(TimeLimitExceeded&) {
        log.status = SolverStatus::TimeLimitExceeded;
    }
    
    log.time = rolex.Elapsed();
    log.ub = ub.arrival;
    
    if(log.status == SolverStatus::DidNotStart)
    if(solved) 
        log.status = SolverStatus::Completed;
    
    return {ub, log};
}

pair<Route, LBFSLog> LBFS(const Instance& instance, Direction dir, Time departure, Time max_travel_time)
{
    DEBUG_ELEM(INFO, "Running the LBFS algorithm with departure {}, traveling at most {}", departure, max_travel_time);

	Stopwatch rolex_run;
    LBFSLog plog;
    Route res;

    
    using PQ = priority_queue<shared_ptr<Label>, vector<shared_ptr<Label>>, decltype([](const shared_ptr<Label>& l, const shared_ptr<Label>& m){
        if(l->back_arrival_time > m->back_arrival_time) return true;
        if(l->back_arrival_time < m->back_arrival_time) return false;
        return l->latest_arrival > m->latest_arrival;
    })>;
    vector<PQ> queue(instance.VertexCount());
    
    auto latest_origin = instance.GetTimeWindow(dir, instance.Origin(dir)).deadline;
    auto step = (latest_origin - departure)/1;
    for(Time x = 0; x < latest_origin-departure; x += step) {
        queue[0].push(make_shared<Label>(VertexSet(), instance.Origin(dir), x, departure+x+step, departure+x+step));
    }
    
    vector<hash_table<VertexSet, Time>> best(instance.VertexCount());
    deque<Label> extended;
    
    for(auto q = 0ul, e = 0ul; e < instance.VertexCount() and res.path.empty();) {
        time_guardian.FailIfTimeLimit();
        
        if(queue[q].empty()) {
            e += 1;
            q = (q+1)%instance.VertexCount();
            continue;
        }
        e = 0;
            
        auto ptr_l = queue[q].top();
        queue[q].pop();
        auto latest_arrival_l = ptr_l->latest_arrival;
        auto &lq = *ptr_l;
        DEBUG_ELEM(TRACE, "\nProcessing label {}", lq);

        if(auto [it, ins] = best[lq.last].insert({lq.visited,lq.travel_time}); not ins) {
            if(it->second <= lq.travel_time) {
                DEBUG_ELEM(TRACE, "\tDominated by a previous label with travel time {}", it->second);
                plog.dominated_count += 1;
                continue;
            } else {
                DEBUG_ELEM(TRACE, "\tDominates a previous label");
                it->second = lq.travel_time;
            }
        } else {
            DEBUG_ELEM(TRACE, "\tNot dominated by a previous label");            
        }
        DEBUG_ELEM(TRACE, "\tVisited: {} + {}", lq.last, lq.visited);
            
        extended.push_back(lq);
        auto& l = extended.back();
        plog.undominated_count += 1;            
              
        for(auto v : instance.Vertices())
        if(v != instance.Origin(dir))
        if(v != l.last)
        if(not l.visited.test(v)) 
        if(departure + l.travel_time <= instance.GetTimeWindow(dir, v).deadline)
        {
            DEBUG_ELEM(TRACE, "\n\nProcessing extension to {}", v);
            plog.enumerated_count += 1;
            
            auto visited = l.visited; 
            visited.set(l.last);
            DEBUG_ELEM(TRACE, "\t\t\tVisited {}", visited);
            Time arrival = instance.ArrivalTime(dir, l.last, v, departure + l.travel_time);
            auto travel_time = arrival - departure;
            DEBUG_ELEM(TRACE, "\t\t\tArrival time: {}, travel time: {}", arrival, travel_time);
                
            if(travel_time > max_travel_time or arrival > instance.GetTimeWindow(dir, v).deadline) {
                DEBUG_ELEM(TRACE, "\t\t\tIgnoring the extension because the travel time {} exceedes the max travel time {} or the arrival {} is outside the time window {}", travel_time, max_travel_time, arrival, instance.GetTimeWindow(dir, v));
                plog.discarded_count += 1;
                continue;
            }
            
            auto unreachable = instance.Unreachable(dir, v, arrival);
            DEBUG_ELEM(TRACE, "\t\t\tUnreachable set: {}", unreachable);
                
            if(not is_subset(unreachable, visited)) {
                DEBUG_ELEM(TRACE, "\t\t\tIngoring the extension because of many unreachable");
                plog.discarded_count += 1;
                continue;
            }

            Label m(visited, v, travel_time, &l);
            DEBUG_ELEM(TRACE, "\t\tComputed the extension {}", m);            
            
            if(auto [it, ins] = best[m.last].insert({m.visited, m.travel_time + 1}); not ins) {
                if(it->second <= m.travel_time) {
                    DEBUG_ELEM(TRACE, "\tDominated by a previous label");
                    plog.dominated_count += 1;
                    continue;
                } else {
                    it->second = m.travel_time + 1;
                }
            }
            
            if(m.last == instance.Destination(dir)) {
                auto r = m.GetRoute(departure);
                if(dir == Direction::Backward) {
                    reverse(r.path.begin(), r.path.end());
                    r.departure = 0;
                    r.arrival = instance.ArrivalTime(r);
                }
                if(r.arrival < max_travel_time)
                {
                    DEBUG_ELEM(VERBOSE, "\tNew route found: {} with undominated count {}", r, plog.undominated_count);
                    res = r;
                    break;
                }
                continue;
            }
                                  
            m.latest_arrival = min(
                instance.ArrivalTime(dir, l.last, v, latest_arrival_l),
                instance.GetTimeWindow(dir, v).deadline
            );
            auto m_route = m.GetRoute();
            reverse(m_route.path.begin(), m_route.path.end());
            m.back_arrival_time = instance.ArrivalTime(
                op(dir), 
                m_route, 
                instance.Horizon() - m.latest_arrival
            );
            ASSERT(ASSERTION, m.back_arrival_time <= max_travel_time, "back arrival time greater than max travel time in feasible path. back_arrival: {} {}", m.back_arrival_time, max_travel_time);

            if(m.back_arrival_time == max_travel_time) {
                DEBUG_ELEM(TRACE, "\t\tDiscarding {} because of the back arrival time {}", m, m.back_arrival_time);
                plog.discarded_count += 1;
                continue;
            }

            DEBUG_ELEM(TRACE, "\t\tPushing extension {}", m);            
            queue[m.visited.count()].push(make_shared<Label>(m));            
        }
        q = (q+1)%instance.VertexCount();
    }
       
    plog.time += rolex_run.Elapsed();    
    return {res, plog};    
}

} // namespace tsptwm

