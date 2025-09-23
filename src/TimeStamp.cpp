#include "TimeStamp.hpp"

TimeStamp::TimeStamp()
          :microSecondsSinceEpoch_(0)
{}
TimeStamp::TimeStamp(uint64_t microSecondsSinceEpoch)
          :microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}
uint64_t TimeStamp::Now()
{
    auto now=std::chrono::system_clock::now();
    auto microSeconds=std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return microSeconds.count();
}
uint64_t TimeStamp::MicroSecondsSinceEpoch() const
{
    return microSecondsSinceEpoch_;
}