#pragma once
#include "Timer.hpp"
#include "TimeStamp.hpp"
#include "Channel.hpp"
#include <set>

class EventLoop;
class TimerQueue
{
public:
    TimerQueue(EventLoop* loop);
    void AddTimer(TimerCallback cb,TimeStamp when,double interval);
private:
    using Entry=std::pair<TimeStamp,Timer*>;
    using TimerList=std::set<Entry>;

    int CreateTimerFd();
    void HandleRead();
    std::vector<Entry> GetExpired(TimeStamp now);
    void Reset(const std::vector<Entry>& expired);
    void Insert(Timer* timer);
    void AddTimerInLoop(TimerCallback cb,TimeStamp when,double interval);
    
    EventLoop* loop_;
    TimerList timers_;
    std::unique_ptr<Channel> channel_;
};