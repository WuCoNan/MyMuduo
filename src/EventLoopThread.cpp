#include "EventLoopThread.hpp"
#include "EventLoop.hpp"

EventLoopThread::EventLoopThread()
    : loop_(nullptr)
{
}

void EventLoopThread::StartEventLoop()
{
    EventLoop loop;

    {
        std::unique_lock<std::mutex> lock(mtx_);
        loop_ = &loop;
        cv_.notify_one();
    }

    loop.Loop();
}

EventLoop *EventLoopThread::GetEventLoop()
{
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]()
                 { return loop_ != nullptr; });
    }
    return loop_;
}

void EventLoopThread::Start()
{
    thread_ = std::move(std::thread([this]()
                                    { StartEventLoop(); }));
}