/*
 * @author Francisco Soulignac
 * @brief Data types for reading the json files.
 * 
 * Based on a design by Gonzalo Lera-Romero
 */

#ifndef TSPTWM_INPUT_H
#define TSPTWM_INPUT_H

#include <format>
#include <vector>
#include "json.hpp"
#include "types.h"

namespace tsptwm
{
    
/**
 * @class TimeWindow
 * @author Francisco Soulignac
 * @brief Time windows within the input and the algorithms
 */
struct TimeWindow {
    Time release = 0;
    Time deadline = -1;
};

void to_json(nlohmann::json& j, TimeWindow tws);
void from_json(const nlohmann::json& j, TimeWindow& tws);

/**
 * @class Input
 * @author Francisco Soulignac
 * @brief The input instance of the TSPTWM with no preprocessing
 * 
 * This class serves to parse different instances and to provide a minimal input
 */
class Input
{
public:
    /** Number of vertices in the instance */
    size_t vertex_count;
    
    /** set of vertices in the network */
    std::vector<Vertex> vertices;
    VertexSet vertex_set;
    
    /** Vertex of the starting depot (usually 0) */   
	Vertex origin; 
    
    /** Vertex of the ending depot (usually n+1) */
    Vertex destination;    
    
    /** Horizon for the travel time functions */
    Time horizon;
            
    /** time[i][j] is the time required to travel from i to j in the forward direction */
    std::vector<std::vector<Time>> time;

    /* time_window[v] is the declared time window for vertex v when traveling in the forward direction */
    std::vector<TimeWindow> time_window;
};
    
/**
 * @brief Serializes an instance into the JSON format.
 * @param j JSON object
 * @param instance to serialize
 */
void to_json(nlohmann::json& j, const Input& input);

/**
 * @brief Parses an instance from the JSON format.
 * @param j JSON object
 * @param instance to load
 */
void from_json(const nlohmann::json& j, Input& input);

} 

/**
 * Format of a time window
 */
template <>
struct std::formatter<tsptwm::TimeWindow> : std::formatter<std::string> {
    auto format(tsptwm::TimeWindow tw, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "[{},{}]", tw.release, tw.deadline);
    }
};

/**
 * Format of an input (mostly for debugging purposes)
 */
template<>
struct std::formatter<tsptwm::Input> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const tsptwm::Input& tsp, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", nlohmann::json(tsp).dump());
    }
};


#endif //TSPTWM_INPUT_H
