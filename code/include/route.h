/*
 * @author Francisco Soulignac
 * @brief To represent (partial) routes
 */

#ifndef TSPTWM_ROUTE_H
#define TSPTWM_ROUTE_H

#include <format>
#include "input.h"

namespace tsptwm
{

/**
 * @class Route
 * @author Francisco
 * @brief Represents a (partial) route of the TSPTW-M network
 * 
 * Together with its path, a route stores a departure and arrival time.
 */
class Route
{
public:
    /** Path taken by the vehicle */
    GraphPath path;  
    
    /** Times of the routes */
	Time departure = 0; 
    Time arrival = 0;
    
    /**
     * @brief Creates a route.
     * @param departure of the route
     * @param arrival of the route
     * @param path of the route
     */
    explicit Route(Time departure = 0, Time arrival = 0, const GraphPath& path = {});

    /**
     * @brief Duration (arrival - departure) of the route
     */
    inline Time Duration() const {return arrival - departure;}
};

/**
 * @brief Serializes a route r into the JSON format.
 * @param j JSON object
 * @param r route to serialize
 */
void to_json(nlohmann::json& j, const Route& r);

} // namespace tsptwm

/**
 * To format a route
 */
template<>
struct std::formatter<tsptwm::Route> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::Route& r, std::format_context& ctx) const {
        std::string s = nlohmann::json(r).dump();
        //return std::formatter<std::string>::format(s, ctx);
        return std::format_to(ctx.out(), "{}", s);
    }
};


#endif //TSPTWM_ROUTE_H
