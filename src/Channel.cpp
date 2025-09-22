#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Poller.hpp"
Channel::Channel(int fd,EventLoop* loop)
                :fd_(fd)
                ,loop_(loop)
                ,tied_(false)
{}
Channel::~Channel()
{
    ::close(fd_);
}

void Channel::SetRevents(int revents)
{
    revents_|=revents;
}

bool Channel::IsReadEvent() const
{
    return events_&(EPOLLIN|EPOLLPRI);
}

bool Channel::IsWriteEvent() const
{
    return events_&EPOLLOUT;
}

bool Channel::IsNoneEvent() const
{
    return events_==0;
}

void Channel::EnableRead()
{
    events_|=EPOLLIN|EPOLLPRI|EPOLLRDHUP;
    loop_->UpdatePoller(this);
}

void Channel::EnableWrite()
{
    events_|=EPOLLOUT|EPOLLRDHUP;
    loop_->UpdatePoller(this);
}

void Channel::DisableRead()
{
    events_&=~(EPOLLIN|EPOLLPRI);
    loop_->UpdatePoller(this);
}

void Channel::DisableWrite()
{
    events_&=~(EPOLLOUT);
    loop_->UpdatePoller(this);
}

void Channel::DisableAll()
{
    events_=0;
    loop_->UpdatePoller(this);
}

int Channel::GetFd() const
{
    return fd_;
}

int Channel::GetEvents() const
{
    return events_;
}

void Channel::SetReadCallback(EventCallback read_callback)
{
    read_callback_=read_callback;
}
void Channel::SetCloseCallback(EventCallback close_callback)
{
    close_callback_=close_callback;
}
void Channel::SetErrorCallback(EventCallback error_callback)
{
    error_callback_=error_callback;
}
void Channel::SetWriteCallback(EventCallback write_callback)
{
    write_callback_=write_callback;
}
void Channel::HandleEvent()
{
    auto guard=tie_.lock();
    if(!guard&&tied_)
        return;

    if(revents_&EPOLLRDHUP)
    {
        if(close_callback_)
            close_callback_();
    }
    if(revents_&(EPOLLERR|EPOLLHUP))
    {
        if(error_callback_)
            error_callback_();
    }
    if(revents_&EPOLLIN)
    {
        if(read_callback_)
            read_callback_();
    }
    if(revents_&EPOLLOUT)
    {
        if(write_callback_)
            write_callback_();
    }
}

void Channel::Tie(const std::shared_ptr<TcpConnection>& obj)
{
    tie_=obj;
    tied_=true;
}

void Channel::Remove()
{
    loop_->RemoveChannel(this);
}