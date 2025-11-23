
#include "stopwatch.h"

using namespace std;

namespace tsptwm {
    
Stopwatch::Stopwatch() 
{
    Reset();
}

void Stopwatch::Reset() {
    start_time = clock::now();
}

Duration Stopwatch::Elapsed() const {
    return clock::now() - start_time;
}

}   //namespace tsptwm

