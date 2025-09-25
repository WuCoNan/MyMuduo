#include "Timer.hpp"

Timer::Timer(TimerCallback cb,TimeStamp when,double interval)
            :cb_(cb)
            ,expiration_(when)
            ,interval_(interval)
{
}

bool Timer::Repeat()
{
    if(interval_==0)
        return false;
    expiration_+=static_cast<uint64_t>(interval_*1000000);
    return true;
}