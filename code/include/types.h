/*
 * @author Francisco Soulignac
 * @brief Basic data types
 */
 
#include <vector>
#include <bitset>
#include <limits>
#include <format>
#include <chrono>
 
#ifndef TSPTWM_TYPES_H
#define TSPTWM_TYPES_H

namespace tsptwm
{
    
#ifdef DEFINE_MAX_N
constexpr unsigned int MAX_N = DEFINE_MAX_N;  /** Maximum number of vertices that an instance can have. */
#else
constexpr unsigned int MAX_N = 127;  /** Maximum number of vertices that an instance can have. */
#endif

/** Basic representation of a set of vertices */
using VertexSet = std::bitset<MAX_N>;   
using Vertex = size_t;

/** Time within the algorithm */
using Time = long long;
constexpr Time infty_time = std::numeric_limits<Time>::max() >> 1;

/** Paths within the algorithm */
using GraphPath = std::vector<Vertex>;

/** 
 * Direction of the arcs with to an input TSP-D network D.
 */
enum class Direction 
{
    Forward,    /**< Arcs are interpreted as given by D */
    Backward    /**< Arcs are interpreted as given by the reverse of D */
};

/**
 * @brief Returns an integer that can be used to index Directions within indexable data structres
 * 
 * @param d the direction to index
 * @return the index of d
 */
inline constexpr int idx(Direction d) {return static_cast<int>(d);}

/**
 * @brief Returns the opposite direction
 * @param d a direction
 * @return the opposite of d
 */
inline constexpr Direction op(Direction d) {return Direction(1-idx(d));}

/**
 * @brief Returns the index of the opposite direction
 * @param d a direction
 * @return idx(op(d))
 */
inline constexpr int opidx(Direction d) {return idx(op(d));}

// Duration of a process
using Duration = std::chrono::high_resolution_clock::duration;

/**
 * @brief returns the duration in seconds
 * @param d a duration
 * @return d in seconds
 */
inline double to_seconds(const Duration& d) {
    return std::chrono::duration<double>(d).count();
}

} //namespace tsptwm

/** 
 * Formatter for directions
 */
template <>
struct std::formatter<tsptwm::Direction> : std::formatter<std::string> {
    auto format(tsptwm::Direction dir, std::format_context& ctx) const {
        auto res = dir == tsptwm::Direction::Forward ? "forward" : "backward";
        return std::format_to(ctx.out(), "{}", res);
    }
};

#endif //TSPTWM_TYPES_H
