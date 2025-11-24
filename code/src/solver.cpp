
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
    
    void set_best_departure(const Instance& instance, Route& r) {
        if(r.arrival < instance.Horizon())
        r.arrival = instance.ArrivalTime(r, r.departure);
        while(instance.ArrivalTime(r, r.departure + 1) == r.arrival) r.departure += 1;
    }
}

tuple<Route, Route, Route, DurationSolverLog> SolveDuration(const Instance& instance, bool ms_only)
{
    DurationSolverLog log;
    Time p = 0, q = 0, last_p = infty_time;
    tsptwm::Route prev_best;
    Route ub(0,infty_time, {}), ms_ub, reverse_ms_ub;

    while(p <= last_p) {
        time_guardian.FailIfTimeLimit();
            
        DEBUG_ELEM(PRINT, "\n\nSolving for departure time {}/{}\n", p, last_p); 
            
        if(not prev_best.path.empty()) {
            prev_best.departure = p;
            prev_best.arrival = instance.ArrivalTime(prev_best, p);
            prev_best = LocalSearch(instance, prev_best);
            if(prev_best.Duration() <= q-p)
                set_best_departure(instance, prev_best);
                    
            DEBUG_ELEM(INFO, "After local search: {}", prev_best);
                    
            if(prev_best.Duration() < ub.Duration()) {
                ub = prev_best;
                DEBUG_ELEM(PRINT, "Updated best solution to {}", ub);
            }
                    
            if(prev_best.Duration() <= q-p) {
                p = prev_best.departure + 1;
                continue;
            }
            
            if(prev_best.arrival >= instance.Horizon())
                prev_best = Route(p, p + ub.Duration(), {});
        }
                        
        auto [fwd_best, fwd_log] = SolveMakespan(instance, prev_best, Direction::Forward, p);
        log.enum_time += fwd_log.time;
                        
        if(fwd_log.status == SolverStatus::Completed) {
            DEBUG_ELEM(ASSERTION, "Found a solution {}", fwd_best);
            
            if(p == 0) {
                log.ms_status = SolverStatus::Completed;
                ms_ub = fwd_best;
            } else if(last_p > instance.Horizon() and fwd_log.ub >= instance.Horizon()) {
                //por si falla el last_t0
                break;
            } 

            prev_best = fwd_best;
            set_best_departure(instance, prev_best);
            if(prev_best.Duration() < ub.Duration()) {
                ub = prev_best;
                DEBUG_ELEM(PRINT, "Updated best solution to {}", ub);
            }

            q = prev_best.arrival;
            DEBUG_ELEM(INFO, "Updated q to {}", q);
        }    
            
        if(p == 0) {
            log.ms_time = time_guardian.Now();
            log.ms_ub = fwd_log.ub; 
            log.ms_preprocess_time = log.preprocess_time;
            log.ms_enum_time = log.enum_time;
            if(ms_only) break;
        }
            
        if(fwd_log.status != SolverStatus::Completed) break;
            
        if(p == 0) {
            Stopwatch rolex_ms_back;
            clog << endl << endl << "Computing the latest feasible departure" << endl; 

            auto [bwd_best, bwd_log] = SolveMakespan(instance, Route(), Direction::Backward, 0);
            log.enum_time += bwd_log.time;

            log.reverse_ms_ub = bwd_log.ub;
            log.reverse_ms_time = rolex_ms_back.Elapsed();

            DEBUG_ELEM(ASSERTION, "Found a solution: {}", bwd_best);
            if(bwd_log.status == SolverStatus::Completed) {
                log.reverse_ms_status = SolverStatus::Completed;                  
                last_p = bwd_best.departure;
                DEBUG_ELEM(PRINT, "Lastest departure: {}", last_p);
                reverse(bwd_best.path.begin(), bwd_best.path.end());
                reverse_ms_ub = bwd_best;
                    
                set_best_departure(instance, bwd_best);
                if(bwd_best.Duration() < ub.Duration()) {
                    ub = bwd_best;
                    DEBUG_ELEM(PRINT, "Updated best solution to {}", ub);
                }
            }
        }

        p = q - ub.Duration() + 1;
        DEBUG_ELEM(INFO, "Updated p to {}", p);
    }
    
    if(p > last_p) {
        log.status = SolverStatus::Completed;
    }
    log.ub = ub.Duration();
    return {ub, ms_ub, reverse_ms_ub, log};
}


pair<Route, MakespanSolverLog> SolveMakespan(
    const Instance& instance, 
    const Route& init_route,
    Direction dir,
    Time dep
) {

    DEBUG_ELEM(INFO, "Solver::InformedSearch()");
    MakespanSolverLog log;

    // Initial upper bound
    auto ub = init_route;
    if(ub.path.empty()) {
        std::iota(ub.path.begin(), ub.path.end(), 0);
        ub.departure = dep;
        ub.arrival = instance.Horizon();
    }
    DEBUG_ELEM(INFO, "ub: {}, duration: {}, instance.horizon {}", ub, ub.Duration(), instance.Horizon());

    // Output to console
    clog << format("{:>12}\t{:<30}{:>12}{:>12}\n", "Time", "Process", "Duration", "BKS");
    auto stream_info = [&](const std::string& process, const Duration& time) {
        clog << format("{:>12.2f}\t{:<30}{:>12.2f}{:>12}\n", to_seconds(time_guardian.Now()), process, to_seconds(time), ub.arrival);
    };
    

    Stopwatch rolex;
    bool solved = false;
    while(not solved) {     
        Route r;
        LBFSLog bfs_log;
        try {
            tie(r, bfs_log) = LBFS(instance, op(dir), instance.Horizon() - ub.Duration(), instance.Horizon() - dep);
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
            reverse(r.path.begin(), r.path.end());
            r.departure = dir == Direction::Forward ? dep : instance.Horizon() - instance.ArrivalTime(dir, r, dep);
            r.arrival = dir == Direction::Forward ? instance.ArrivalTime(dir, r) : instance.Horizon() - dep;
            ub = r;
            DEBUG_ELEM(INFO, "DFSExact: Improved elementary route {}", r);
        } else {
            DEBUG_ELEM(INFO, "DFS Enumeration completed");
            solved = true;
        }
        stream_info(format("DFSEnumerate {}", ub.Duration()), bfs_log.time);
    }
    log.time = rolex.Elapsed();
    log.ub = ub.Duration();
    
    if(log.status == SolverStatus::DidNotStart)
    if(solved) 
        log.status = SolverStatus::Completed;
    
    return {ub, log};
}

pair<Route, LBFSLog> LBFS(const Instance& instance, Direction dir, Time departure, Time max_arrival_time)
{
    DEBUG_ELEM(INFO, "Running the LBFS algorithm with departure {}, traveling at most until {}", departure, max_arrival_time);

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
                
            if(arrival > max_arrival_time or arrival > instance.GetTimeWindow(dir, v).deadline) {
                DEBUG_ELEM(TRACE, "\t\t\tIgnoring the extension because the arrival time {} exceedes the max travel time {} or the arrival {} is outside the time window {}", travel_time, max_arrival_time, arrival, instance.GetTimeWindow(dir, v));
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
            ASSERT(ASSERTION, m.back_arrival_time <= max_arrival_time - departure, "back arrival time {} greater than max arrival time - departure ({}-{}) {} in feasible path {}.", m.back_arrival_time, max_arrival_time, departure, max_arrival_time-departure, m);

            if(m.back_arrival_time == max_arrival_time - departure) {
                DEBUG_ELEM(TRACE, "\t\tDiscarding {} because of the back arrival time {}", m, m.back_arrival_time);
                plog.discarded_count += 1;
                continue;
            }

            DEBUG_ELEM(TRACE, "\t\tPushing extension {}", m);            
            if(m.last == instance.Destination(dir)) 
            {
                res = m.GetRoute(departure);
                DEBUG_ELEM(VERBOSE, "\tNew route found: {} with undominated count {}", res, plog.undominated_count);
                break;
            }
            queue[m.visited.count()].push(make_shared<Label>(m));            
        }
        q = (q+1)%instance.VertexCount();
    }
       
    plog.time += rolex_run.Elapsed();    
    return {res, plog};    
}

} // namespace tsptwm

