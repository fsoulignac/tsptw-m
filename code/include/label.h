/*
 * @author Francisco Soulignac
 * @brief Label for the labeling algorithms
 */

#ifndef TSPTWM_LABEL_H
#define TSPTWM_LABEL_H

#include <format>
#include "types.h"
#include "route.h"
#include "print.h"

namespace tsptwm
{
    
/**
 * @class Label
 * @author Francisco
 * @brief This class implements the labels that allow the enumeration and reconstruction of partial routes 
 * within the labeling algorithms
 */
class Label
{
public:
    /** Set of customers that the vehicle visted already */
    VertexSet visited;
        
    /** last vertex of the partial route */
    Vertex last = 0;
    
    /** travel time to reach vertex last */
    Time travel_time = 0;
        
    /** backward arrival time to the origin departing from the release window of last */
    Time back_arrival_time = infty_time;
    
    /** latest possible arrival time to reach vertex last */
    Time latest_arrival = 0;
    
    /** Label obtained by removing last vertex; needed for reconstructing the truck and drone paths */
    const Label* prev = nullptr; 
    
    Label() = default;
    
    /** Constructors to emplace back */
    Label(VertexSet V, Vertex v, Time tt, Time back_at, Time late_a) :
    visited(V), last(v), travel_time(tt), back_arrival_time(back_at), latest_arrival(late_a) {}

    Label(VertexSet V, Vertex v, Time tt, const Label* p = nullptr) :
    visited(V), last(v), travel_time(tt), prev(p) {}

    /**
     * @brief returns the partial route associated to the label.  
     * 
     * @param departure: real departing time
     * @return partial route
     */
    Route GetRoute(Time departure = 0) const;
};

using LabelPtr = std::shared_ptr<const Label>;    

} // namespace tsptwm

/**
 * To format labels (mostly for debugging purposes)
 */
 
template<>
struct std::formatter<tsptwm::Label> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::Label& l, std::format_context& ctx) const {
        return std::format_to(ctx.out(), 
            "(length: {}, last: {}, travel time: {}, back_arrival_time: {}, latest_arrival: {}, visited: {}) -> {}", 
            l.visited.count(), l.last, l.travel_time, l.back_arrival_time, l.latest_arrival, l.visited, l.GetRoute()
        );
    }
};


#endif //TSPTWM_LABEL_H
