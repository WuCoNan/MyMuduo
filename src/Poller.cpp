#include "Poller.hpp"
#include "Channel.hpp"
Poller::Poller(EventLoop* loop)
              :epfd_(CreateEpoll())
              ,events_(max_event_size_)
{}

int Poller::CreateEpoll()
{
    return epoll_create1(EPOLL_CLOEXEC);
}

void Poller::Poll(ChannelList* active_channels)
{
    int num=epoll_wait(epfd_,&*events_.begin(),max_event_size_,-1);

    for(int i=0;i<num;i++)
    {
        Channel* active_channel=static_cast<Channel*>(events_[i].data.ptr);
        auto revents=events_[i].events;

        active_channel->SetRevents(revents);
        active_channels->push_back(active_channel);
        
    }

}

void Poller::Update(Channel* channel)
{
    if(channels_.find(channel)==channels_.end())
    {
        channels_.emplace(channel);
        Update(channel,EPOLL_CTL_ADD);
    }
    else
    {
        if(channel->IsNoneEvent())
            Update(channel,EPOLL_CTL_DEL);
        else
            Update(channel,EPOLL_CTL_MOD);
    }
}

void Poller::Remove(Channel* channel)
{
    channels_.erase(channel);
}
void Poller::Update(Channel* channel,int operation)
{
    epoll_event event;
    event.events=channel->GetEvents();
    event.data.ptr=channel;

    epoll_ctl(epfd_,operation,channel->GetFd(),&event);
}