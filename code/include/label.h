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

class Label;
using LabelPtr = std::shared_ptr<const Label>;    
    
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
    Time earliest_arrival_time = 0;

    /** latest possible arrival time to reach vertex last */
    Time latest_arrival_time = 0;
        
    /** backward arrival time from earliest possible arrival */
    Time back_from_earliest_time = infty_time;

    /** backward arrival time from latest possible arrival */
    Time back_from_latest_time = infty_time;    
    
    /** Label obtained by removing last vertex; needed for reconstructing the truck and drone paths */
    LabelPtr prev = nullptr; 
    
    Label() = default;
    
    /** Constructors to emplace back */
    Label(VertexSet V, Vertex v, Time eat, Time lat, Time back_eat, Time back_lat) :
    visited(V), last(v), earliest_arrival_time(eat), latest_arrival_time(lat), back_from_earliest_time(back_eat), back_from_latest_time(back_lat), prev(nullptr) {}

    Label(VertexSet V, Vertex v, Time eat, Time lat, const LabelPtr& p) :
    visited(V), last(v), earliest_arrival_time(eat), latest_arrival_time(lat), prev(p) {}

    /**
     * @brief returns the partial route associated to the label.  
     * 
     * @param departure: real departing time
     * @return partial route
     */
    Route GetRoute(Time departure = 0) const;
};

} // namespace tsptwm

/**
 * To format labels (mostly for debugging purposes)
 */
 
template<>
struct std::formatter<tsptwm::Label> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::Label& l, std::format_context& ctx) const {
        return std::format_to(ctx.out(), 
            "(length: {}, last: {}, earliest arrival time: {}, latest_arrival_time: {}, back_from_earliest_time: {}, back_from_latest_time: {}, visited: {}) -> {}", 
            l.visited.count(), l.last, l.earliest_arrival_time, l.latest_arrival_time, l.back_from_earliest_time, l.back_from_latest_time, l.visited, l.GetRoute()
        );
    }
};


#endif //TSPTWM_LABEL_H
