#pragma once
#include "Noncopyable.hpp"
#include "Callbacks.hpp"
#include "TimeStamp.hpp"
class Timer : public Noncopyable
{
public:
    Timer(TimerCallback cb,TimeStamp when,double interval);
    void Run() {cb_();}
    bool Repeat();
    TimeStamp Expiration() const {return expiration_;}
    double Interval() const {return interval_;}
private:
    TimerCallback cb_;
    TimeStamp expiration_;
    double interval_;
};