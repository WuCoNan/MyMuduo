#pragma once
#include "Noncopyable.hpp"
#include "Callbacks.hpp"
#include "EventLoopThreadPool.hpp"

#include <memory>
#include <netinet/in.h>
#include <unordered_set>

class EventLoop;
class Acceptor;
class EventLoopThreadPool;
class TcpConnection;
class TcpServer : public Noncopyable
{
public:
    TcpServer(EventLoop *mainloop);
    ~TcpServer(){}
    void Start();
    void SetSubLoopNum(int num);
    void SetMessageCallback(MessageCallback cb);

private:
    std::unique_ptr<Acceptor> acceptor_;
    EventLoop *loop_;
    std::shared_ptr<EventLoopThreadPool> pool_;
    std::unordered_set<std::shared_ptr<TcpConnection>> conns_;
    MessageCallback message_cb_;

    void NewConnection(int, sockaddr);
    void RemoveConnection(const std::shared_ptr<TcpConnection>&);
    void RemoveConnctionInLoop(const std::shared_ptr<TcpConnection>&);
};
