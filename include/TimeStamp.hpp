#pragma once
#include "Noncopyable.hpp"
#include <cstdint>
#include <ctime>
#include <chrono>
class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(uint64_t microSecondsSinceEpoch);
    static uint64_t Now();
    uint64_t MicroSecondsSinceEpoch() const;
    bool operator==(const TimeStamp& other) const {return microSecondsSinceEpoch_==other.microSecondsSinceEpoch_;}
    bool operator<(const TimeStamp& other) const {return microSecondsSinceEpoch_<other.microSecondsSinceEpoch_;}
    bool operator>(const TimeStamp& other) const {return other<*this;}
    bool operator<=(const TimeStamp& other) const {return !(*this>other);}
    bool operator>=(const TimeStamp& other) const {return !(*this<other);}
    TimeStamp operator+(const TimeStamp& other) const {return TimeStamp(microSecondsSinceEpoch_+other.microSecondsSinceEpoch_);}
    TimeStamp operator+(uint64_t microSecondsInterval) const {return TimeStamp(microSecondsInterval+microSecondsSinceEpoch_);}
    TimeStamp& operator+=(uint64_t microSecondsInterval) {microSecondsSinceEpoch_+=microSecondsInterval;return *this;}
    TimeStamp& operator+=(const TimeStamp& other) {microSecondsSinceEpoch_+=other.microSecondsSinceEpoch_;return *this;}
private:
    uint64_t microSecondsSinceEpoch_;

};
