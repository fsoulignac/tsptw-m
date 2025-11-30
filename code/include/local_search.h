/*
 * @author Francisco Soulignac
 * @brief Local search method
 */

#ifndef TSPTWM_LOCAL_SEARCH_H
#define TSPTWM_LOCAL_SEARCH_H

#include "route.h"
#include "instance.h"

namespace tsptwm {
    
Route LocalSearch(const Instance& instance, const Route& route, bool from_departure = true);

}  // namespace tsptwm

#endif //TSPTWM_LOCAL_SEARCH_H
