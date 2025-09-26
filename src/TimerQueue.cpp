#include "TimerQueue.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/timerfd.h>
#include "EventLoop.hpp"

TimerQueue::TimerQueue(EventLoop* loop)
                      :loop_(loop)
                      ,channel_(new Channel(CreateTimerFd(),loop))
{
    channel_->SetReadCallback(std::bind(&TimerQueue::HandleRead,this));
    channel_->EnableRead();
}

int TimerQueue::CreateTimerFd()
{
    return ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
}

void TimerQueue::HandleRead()
{
    TimeStamp now(TimeStamp::Now());

    auto expired=GetExpired(now);

    for(auto& entry:expired)
    {
        if(cancel_timers_.find(entry.second->Sequence())==cancel_timers_.end())
            entry.second->Run();
    }

    Reset(expired,now);
}

std::vector<TimerQueue::Entry> TimerQueue::GetExpired(TimeStamp now)
{
    std::vector<Entry> expired;

    auto iter=timers_.upper_bound(Entry{now,reinterpret_cast<Timer*>(UINTPTR_MAX)});
    std::copy(timers_.begin(),iter,std::back_inserter(expired));
    timers_.erase(timers_.begin(),iter);

    return expired;
}

void TimerQueue::Reset(const std::vector<Entry>& expired,TimeStamp now)
{
    if(!timers_.empty())
    {
        SetTimerFd(timers_.begin()->second,now);
    }

    for(auto& entry:expired)
    {
        if(entry.second->Repeat()&&cancel_timers_.find(entry.second->Sequence())==cancel_timers_.end())
            Insert(entry.second);
        else 
        {
            if(cancel_timers_.find(entry.second->Sequence())!=cancel_timers_.end())
                cancel_timers_.erase(entry.second->Sequence());

            delete entry.second;
        } 
    }

}

void TimerQueue::Insert(Timer* timer)
{
    if(timers_.empty()||timer->Expiration()<timers_.begin()->first)
    {
        TimeStamp now(TimeStamp::Now());
        if(now>=timer->Expiration())
        {
            timer->Run();
            if(!timer->Repeat())
                return;
        }
             
        SetTimerFd(timer,now);
    }

    Entry entry{timer->Expiration(),timer};
    timers_.emplace(entry);
}

TimerId TimerQueue::AddTimer(TimerCallback cb,TimeStamp when,double interval)
{
    Timer *timer=new Timer(cb,when,interval);
    loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop,this,timer));
    return timer->Sequence();
}

void TimerQueue::AddTimerInLoop(Timer* timer)
{
    Insert(timer);
}

void TimerQueue::SetTimerFd(Timer* timer,TimeStamp now)
{
    itimerspec itm;
    itm.it_value.tv_sec=(timer->Expiration().MicroSecondsSinceEpoch()-now.MicroSecondsSinceEpoch())/1000000;
    itm.it_value.tv_nsec=(timer->Expiration().MicroSecondsSinceEpoch()-now.MicroSecondsSinceEpoch())%1000000*1000;
    itm.it_interval.tv_sec=static_cast<uint64_t>(timer->Interval());
    itm.it_interval.tv_nsec=static_cast<uint64_t>(timer->Interval()*1000000000)%1000000000;

    ::timerfd_settime(channel_->GetFd(),0,&itm,0);
}

void TimerQueue::CancelTimer(TimerId timer_id)
{
    loop_->RunInLoop(std::bind(&TimerQueue::CancelTimerInLoop,this,timer_id));
}

void TimerQueue::CancelTimerInLoop(TimerId timer_id)
{
    cancel_timers_.emplace(timer_id);
}