
#include "time_guardian.h"

using namespace std;

namespace tsptwm {

TimeGuardian time_guardian;    
        
void TimeGuardian::SetTimeLimit(const Duration& limit) 
{
    time_limit = limit;
}
    
void TimeGuardian::FailIfTimeLimit() const
{
    if(rolex.Elapsed() > time_limit) 
        throw TimeLimitExceeded();
}

Duration TimeGuardian::Now() const
{
    return rolex.Elapsed();
}
    
    
}
