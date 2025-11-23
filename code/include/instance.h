/*
 * @author Francisco Soulignac
 * @brief Instances in memory that can be modified by the different algorithms
 */

#ifndef TSPTWM_INSTANCE_H
#define TSPTWM_INSTANCE_H

#include <vector>
#include "input.h"
#include "route.h"

namespace tsptwm {

/**
 * @class Instance
 * @author Francisco
 * @brief Represents a dynamic instance of the TSPTW-M and it also serves as a wrapper for many usefull functions
 * 
 * The main purpose of an instance is to represent an instance of the TSPTW-M that we can change.  In this way,
 * we keep a single input of the problem (@see Input) and many instances in a solver.
 * 
 * To avoid duplicating information, each instance is linked to an input of the TSPTW-M and to its reverse
 * These members must survive the instance to avoid access to dangling pointers
 */
class Instance {
public:
    
    Instance() = default;
    
    /**
     * @brief Creates an instance for the TSPTW-M input tsp
     * 
     * @param tsp the instance of the TSPTW-M
     * 
     * @return a new subinstance
     * \pre tsp must survive the lifetime of the subinstance created.
     */
    explicit Instance(const Input* tsp);

    /**
     * @brief Returns the input
     * 
     * Try not to use this function; wrapper methods are provided in this class for convenience.
     * 
     * @return the tsp instance
     */
    inline const Input* TSP() const {return tsp;}
        
    /**
     * @brief Returns the starting depot of the network in direction d
     * @param d direction 
     * @return the starting depot
     */
    inline Vertex Origin(Direction d = Direction::Forward) const 
    {
        return d == Direction::Forward ? TSP()->origin : TSP()->destination;
    }
    
    /**
     * @brief Returns the ending depot of the network in direction d
     * @param d direction 
     * @return the ending depot
     */
    inline Vertex Destination(Direction d = Direction::Forward) const 
    {
        return d == Direction::Forward ? TSP()->destination : TSP()->origin;
    };
    
    /**
     * @brief Returns the planning horizon
     * @return horizon
     */
    inline Time Horizon() const {return TSP()->horizon;}
    
    /**
     * @brief Returns the set of vertices of the network
     * @return set of vertices of the network
     */
    inline const VertexSet& VertexBitset() const {return TSP()->vertex_set;}
        
    /**
     * @brief Returns an iterable of the vertices
     * @return vertices of the network
     */
    inline const std::vector<Vertex>& Vertices() const {return TSP()->vertices;}
    
    /**
     * @brief Number of vertices in the network
     * @return number of vertices in the network
     */
    inline size_t VertexCount() const {return TSP()->vertex_count;}
    
    /**
     * @brief Returns true if the route can be traversed in direction d in the network defined by the instance,
     * leaving at time t.
     * 
     * @param d direction 
     * @param route route to traverse
     * @param t time of departure
     * @return true if the route can be traversed in the network defined by the instance
     */
    inline bool HasRoute(Direction d, const Route& route, Time t = 0) const {
        return ArrivalTime(d, route, t) < infty_time;        
    }
    inline bool HasRoute(const Route& route, Time t = 0) const {return HasRoute(Direction::Forward, route, t);}
        
    /**
     * @brief Returns the time window of v when traversing in direction d
     * 
     * @param d direction
     * @param v vertex
     * @return time window of v
     */
    inline TimeWindow GetTimeWindow(Direction d, Vertex v) const {
        return d == Direction::Forward ? window[v] : TimeWindow{Horizon() - window[v].deadline, Horizon() - window[v].release};
    }
    inline TimeWindow GetTimeWindow(Vertex v) const {
        return window[v];
    }
    
    /**
     * @brief Returns the time required to traverse the arc (v,w) in direction d.
     * 
     * @param d direction 
     * @param v vertex from
     * @param w vertex to
     * @return time required by the truck to traverse (v,w)
     */
    inline Time TravelTime(Direction d, Vertex v, Vertex w) const {
        return d == Direction::Forward ? TSP()->time[v][w] : TSP()->time[w][v];
    }
    inline Time TravelTime(Vertex v, Vertex w) const {
        return TSP()->time[v][w];
    }
            
    /**
     * @brief Returns the arrival time when the arc (v,w) is traversed in direction d departing at time t
     * 
     * It returns infty_time if the visit to v or w is outside its time window
     * 
     * @param d direction 
     * @param v vertex from
     * @param w vertex to
     * @param t departure time
     * @return arrival time a w when departing from v at time t in direction d
     */
    Time ArrivalTime(Direction d, Vertex v, Vertex w, Time t) const;
    
    
    /**
     * @brief Returns the arrival time of route r when traversed in direction d departing at time t
     * 
     * It returns infty_time if any vertex is visited outside its time window
     * 
     * @param d direction 
     * @param r route to traverse
     * @param t departure time
     * @return arrival time a w when departing from v at time t in direction d
     */
    Time ArrivalTime(Direction d, const Route& r, Time t = 0.0) const;
    inline Time ArrivalTime(const Route& r, Time t = 0.0) const {return ArrivalTime(Direction::Forward, r, t);}
    
    /**
     * @brief Returns the vertices that cannot be visited after v in direction d when the visit to v is at time t
     * 
     * @param dir direction
     * @param v vertex
     * @param t visit time to v
     * @return vertices that cannot be visited after v in direction d if the visit to v is at time t.
     */
    const VertexSet& Unreachable(Direction dir, Vertex v, Time t) const;

private:

    const Input* tsp;       // the TSPTWM instance
    std::vector<TimeWindow> window;  //ventana en la que hay que visitar a v
    std::vector<std::vector<std::pair<Time, VertexSet>>> unreachable[2];     //unreachable[v] = (t_1, V_1), ..., (t_k, V_k) with t_i < t_{i+1} such that V_i set of unreachable if v is reached after t_i

    // To update the time window of v in direction d
    bool UpdateTimeWindow(Direction d, Vertex v, TimeWindow wdw);
    inline bool UpdateTimeWindow(Vertex v, TimeWindow wdw) {
        return UpdateTimeWindow(Direction::Forward, v, wdw);
    }
                    
    // Computes the unreachable function and shrinks the time windows
    void UpdatePredecessors();

};
    
}  // namespace tsptwm

#endif //TSPTWM_INSTANCE
