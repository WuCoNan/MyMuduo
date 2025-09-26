#pragma once
#include "Noncopyable.hpp"
#include "Callbacks.hpp"
#include "TimeStamp.hpp"
#include <atomic>

using TimerId=uint64_t;

class Timer : public Noncopyable
{
public:
    Timer(TimerCallback cb, TimeStamp when, double interval);
    void Run() { cb_(); }
    bool Repeat();
    TimeStamp Expiration() const { return expiration_; }
    double Interval() const { return interval_; }
    TimerId Sequence() const {return sequence_;}
private:
    TimerCallback cb_;
    TimeStamp expiration_;
    double interval_;
    static std::atomic<TimerId> Sequence_;
    TimerId sequence_;
};
