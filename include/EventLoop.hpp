#pragma once
#include "Acceptor.hpp"
#include "Poller.hpp"
#include "Noncopyable.hpp"
#include "Channel.hpp"
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
class EventLoop : public Noncopyable
{
public:
    using PendingFunc = std::function<void()>;

    EventLoop();
    void Loop();
    void RunInLoop(PendingFunc);
    void UpdatePoller(Channel*);
    void RemoveChannel(Channel*);
    bool IsInLoopThread() const;
    
private:
    using ChannelList=std::vector<Channel*>;

    std::unique_ptr<Poller> poller_;
    std::vector<PendingFunc> pending_functors_;
    std::atomic_bool quit_;
    std::mutex mtx_;
    const std::thread::id pid_;
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    
    void DoPendingFuncs();
    void QueueInLoop(PendingFunc);
    int CreateEventFd();
    void WakeUp();
};