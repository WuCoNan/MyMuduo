#pragma once
#include "Timer.hpp"
#include "TimeStamp.hpp"
#include "Channel.hpp"
#include <set>
#include <unordered_set>
class EventLoop;
class TimerQueue
{
public:
    TimerQueue(EventLoop* loop);
    TimerId AddTimer(TimerCallback cb,TimeStamp when,double interval);
    void CancelTimer(TimerId timer_id);
private:
    using Entry=std::pair<TimeStamp,Timer*>;
    using TimerList=std::set<Entry>;
    using CancelTimerIdSet=std::unordered_set<TimerId>;

    int CreateTimerFd();
    void HandleRead();
    std::vector<Entry> GetExpired(TimeStamp now);
    void Reset(const std::vector<Entry>& expired,TimeStamp now);
    void Insert(Timer* timer);
    void AddTimerInLoop(Timer* timer);
    void SetTimerFd(Timer* timer,TimeStamp now);
    void CancelTimerInLoop(TimerId timer_id);

    EventLoop* loop_;
    TimerList timers_;
    std::unique_ptr<Channel> channel_;
    CancelTimerIdSet cancel_timers_;
};