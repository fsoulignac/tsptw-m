/*
 * @author Francisco Soulignac
 * @brief The simple methods for solving the TSPTW-M and the TSPTW-D
 */

#ifndef TSPTWM_SOLVER_H
#define TSPTWM_SOLVER_H

#include "instance.h"
#include "log.h"

namespace tsptwm
{

    std::tuple<Route, Route, Route, DurationSolverLog> SolveDuration(const Instance& instance, bool ms_only);
    
    
    /**
     * @brief The solver for the TSPTW-M
     * 
     * Given an instance D and a route init, it returns a route solving the TSPTW-M with duration lower than
     * init.duration (if such a route exists)
     * 
     * The algorithm raises a TimeLimitException according to the time guardian (@see TimeGuardian)
     * For simplicity, the detection is not immediate; instead it is executed from time to time.
     * Consequently, some time can be wasted until the algorithm stops.  Nevertheless, for the purposes
     * of the paper this is enough, as all we want to know is if the time limit is reached.
     * 
     * @param instance the transport network D
     * @param init: the initial route
	 * @return a route R and the log of the execution.  If there exists the desired route, then R is such a route; otherwise
     * R is an empty route.
     */
    std::pair<Route, MakespanSolverLog> SolveMakespan(
        const Instance& instance, 
        const Route& init = Route(), 
        Direction d = Direction::Forward,
        Time dep = 0
    );

    /**
     * @brief The labeling best first search method.
     * 
     * Given an instance D, a departure time t, and an horizon T, it decides if there exists a feasible route
     * R in direction dir that can reach the end depot before T when departing the start depot a time t.
     * 
     * The algorithm raises a TimeLimitException according to the time guardian (@see TimeGuardian)
     * For simplicity, the detection is not immediate; instead it is executed from time to time.
     * Consequently, some time can be wasted until the algorithm stops.  Nevertheless, for the purposes
     * of the paper this is enough, as all we want to know is if the time limit is reached.
     * 
     * @param instance the transport network D
     * @param dir the direction of traversal
     * @param departure time t of departure
     * @param max_travel_time horizon T
	 * @return a route R and the log of the execution.  If there exists a feasible route, then R is such a route; otherwise
     * R is an empty route.
     */
    std::pair<Route, LBFSLog> LBFS(const Instance& instance, Direction dir, Time departure, Time max_arrival_time);

} // namespace tsptwm


#endif //TSPTWM_SOLVER_H
