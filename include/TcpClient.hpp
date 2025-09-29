#pragma once
#include "Noncopyable.hpp"
#include "TcpConnection.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

class EventLoop;
class TcpClient : public Noncopyable
{
public:
    TcpClient(EventLoop *loop);
    void Connect();
    void DisConnect();
    void SetMessageCallback(MessageCallback cb) { messageCallback_ = cb; }
    void SetOnConnectionCallback(OnConnectionCallback cb) { onConnectionCallback_ = cb; }

private:
    EventLoop *loop_;
    std::shared_ptr<TcpConnection> conn_;
    MessageCallback messageCallback_;
    OnConnectionCallback onConnectionCallback_;

    int clientFd_;

    void RemoveConnection(const std::shared_ptr<TcpConnection> &conn);
    void RemoveConnectionInLoop(const std::shared_ptr<TcpConnection> &conn);
    int CreateClientFd() { return ::socket(AF_INET, SOCK_CLOEXEC | SOCK_NONBLOCK | SOCK_STREAM, IPPROTO_TCP); };
};