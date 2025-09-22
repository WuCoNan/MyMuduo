#pragma once
#include "Noncopyable.hpp"
#include <memory>
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
class EventLoop;
class Channel;

class Acceptor : public Noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int, sockaddr)>;
    Acceptor(EventLoop *loop);
    void SetNewConnCallback(NewConnectionCallback);
    void Listen();

private:
    EventLoop *loop_;
    int accept_fd_;
    std::unique_ptr<Channel> accept_channel_;
    NewConnectionCallback newconn_callback_;
    static constexpr int max_accept_once_ = 1024;

    int CreateAcceptFd();
    void HandleRead();
};