#include "EventLoopThreadPool.hpp"
#include "EventLoopThread.hpp"


EventLoopThreadPool::EventLoopThreadPool()
                    :next_(0)
{
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
    auto next_loop=loops_[next_];
    if(++next_==num_)
        next_=0;
    return next_loop;
}

void EventLoopThreadPool::SetNum(int num)
{
    num_=num;
}

void EventLoopThreadPool::Start()
{
    for(int i=0;i<num_;i++)
    {
        eventloop_threads_.emplace_back(std::make_unique<EventLoopThread>());
        eventloop_threads_[i]->Start();
        loops_.emplace_back(eventloop_threads_[i]->GetEventLoop());
    }
    
}