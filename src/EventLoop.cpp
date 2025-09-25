#include "EventLoop.hpp"

thread_local EventLoop* loop_in_this_thread=nullptr;

EventLoop::EventLoop()
          :wakeup_fd_(CreateEventFd())
          ,wakeup_channel_(new Channel(wakeup_fd_,this))
          ,pid_(std::this_thread::get_id())
          ,poller_(new Poller(this))
          ,timer_queue_(new TimerQueue(this))      
{
    if(loop_in_this_thread)
    {
        exit(-1);
    
    }
    loop_in_this_thread=this;

    wakeup_channel_->EnableRead();

}
void EventLoop::Loop()
{
    quit_ = false;
    while (!quit_)
    {
        ChannelList active_channels;
        poller_->Poll(&active_channels);
        for (auto &active_channel : active_channels)
        {
            active_channel->HandleEvent();
        }

        DoPendingFuncs();
    }
}
void EventLoop::DoPendingFuncs()
{
    std::vector<PendingFunc> pending_funcs;
    {
        std::unique_lock<std::mutex> lock(mtx_);
        std::swap(pending_funcs, pending_functors_);
    }
    for (auto &pending_func : pending_funcs)
    {
        pending_func();
    }
}

void EventLoop::RunInLoop(PendingFunc func)
{
    if (IsInLoopThread())
        func();
    else
        QueueInLoop(func);
}

bool EventLoop::IsInLoopThread() const
{
    return pid_ == std::this_thread::get_id();
}

void EventLoop::QueueInLoop(PendingFunc func)
{
    std::unique_lock<std::mutex> lock(mtx_);
    pending_functors_.push_back(func);
    WakeUp();
}

int EventLoop::CreateEventFd()
{
    return ::eventfd(0,EFD_CLOEXEC|EFD_NONBLOCK);
}

void EventLoop::WakeUp()
{
   uint64_t val;
   ::write(wakeup_fd_,&val,sizeof(val)); 
}

void EventLoop::UpdatePoller(Channel* channel)
{
    poller_->Update(channel);
}
void EventLoop::RemoveChannel(Channel* channel)
{
    poller_->Remove(channel);
}

void EventLoop::RunAt(TimerCallback cb,TimeStamp time)
{
    timer_queue_->AddTimer(cb,time,0);
}

void EventLoop::RunAfter(TimerCallback cb,double delay)
{
    TimeStamp when(TimeStamp::Now());
    when+=static_cast<uint64_t>(delay*1000000);
    timer_queue_->AddTimer(cb,when,0);
}

void EventLoop::RunEvery(TimerCallback cb,double interval)
{
    TimeStamp when(TimeStamp::Now());
    when+=static_cast<uint64_t>(interval*1000000);
    timer_queue_->AddTimer(cb,when,interval);
}