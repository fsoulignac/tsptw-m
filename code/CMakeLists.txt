cmake_minimum_required(VERSION 3.22.1)

project("tsptwm" VERSION 1.0 DESCRIPTION "The traveling salesman problem with time windows (makespan version)")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -m64")
if ("${CMAKE_BUILD_TYPE}" MATCHES release OR "${CMAKE_BUILD_TYPE}" MATCHES Release)
    message("-- Compiling in Release Mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -O3 -DNDEBUG")
elseif ("${CMAKE_BUILD_TYPE}" MATCHES debug OR "${CMAKE_BUILD_TYPE}" MATCHES Debug)
    message("-- Compiling in Debug Mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# If the compilation is called from RUNNER then set the macro constant RUNNER.
if ("${RUNNER}" MATCHES ON)
    message("-- Runner compilation")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRUNNER")
endif()

# Add include directory.
include_directories("include/")

# Create binaries.
set(N_VALUES 63 127 255 511)
foreach(N ${N_VALUES})
    add_library(${PROJECT_NAME}${N})
        target_sources(${PROJECT_NAME}${N} PRIVATE  
        src/debug_utils.cpp 
        src/input.cpp 
        src/instance.cpp 
        src/label.cpp
        src/local_search.cpp
        src/log.cpp
        src/route.cpp 
        src/solver.cpp 
        src/stopwatch.cpp
        src/time_guardian.cpp
    )
    target_compile_options(${PROJECT_NAME}${N} PRIVATE -Werror -Wall -Wextra -Wpedantic -Wnull-dereference -Wcast-qual -Wlogical-op -Wduplicated-cond -Wshadow -Wconversion)
    target_compile_definitions(${PROJECT_NAME}${N} PRIVATE DEFINE_MAX_N=${N})
    
    add_executable(tsptw${N} src/main.cpp)
    target_link_libraries(tsptw${N} PUBLIC ${PROJECT_NAME}${N})
    target_compile_definitions(tsptw${N} PRIVATE DEFINE_MAX_N=${N})
    
endforeach()