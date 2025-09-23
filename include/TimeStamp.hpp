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
private:
    uint64_t microSecondsSinceEpoch_;

};
