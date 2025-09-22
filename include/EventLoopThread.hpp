#pragma once
#include "Noncopyable.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>
class EventLoop;

class EventLoopThread : public Noncopyable
{
public:
    EventLoopThread();
    EventLoop *GetEventLoop();
    void Start();

private:
    EventLoop *loop_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread thread_;
    void StartEventLoop();
};