#pragma once
#include "Noncopyable.hpp"
#include "EventLoopThread.hpp"

#include <vector>
#include <memory>
//class EventLoopThread;
class EventLoop;

class EventLoopThreadPool:Noncopyable
{
public:
    EventLoopThreadPool();
    EventLoop *GetNextLoop();
    void SetNum(int num);
    void Start();

private:
    int num_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> eventloop_threads_;
    std::vector<EventLoop *> loops_;
};