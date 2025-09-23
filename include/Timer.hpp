#pragma once
#include "Noncopyable.hpp"
#include "Callbacks.hpp"
#include "TimeStamp.hpp"
class Timer : public Noncopyable
{
public:
    Timer(TimerCallback cb,TimeStamp when,double interval);
private:
    TimerCallback cb_;

};