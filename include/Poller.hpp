#pragma once
#include "Noncopyable.hpp"
#include <sys/epoll.h>
#include <vector>
#include <unordered_set>
class Channel;
class EventLoop;

class Poller : public Noncopyable
{
public:
    using ChannelList = std::vector<Channel *>;
    using EventList=std::vector<epoll_event>;
    using Channels=std::unordered_set<Channel*>;

    Poller(EventLoop *loop);
    void Poll(ChannelList *active_channels);
    void Update(Channel*);
    void Remove(Channel*);
private:
    int epfd_;
    static constexpr int max_event_size_ = 10;
    EventList events_;
    Channels channels_;

    int CreateEpoll();
    void Update(Channel*,int operation);
};