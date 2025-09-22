#pragma once
#include "Noncopyable.hpp"
#include "Callbacks.hpp"
#include "Buffer.hpp"
#include <memory>
#include <atomic>

class EventLoop;
class Channel;

class TcpConnection :public Noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop, int fd);
    ~TcpConnection();
    void ConnectionEstablished();
    void ConnectionDestroyed();
    void SetMessageCallback(MessageCallback);
    void SetCloseCallback(CloseCallback);
    void Send(const char* data,size_t len);
    void SendInLoop(const char* data,size_t len);
    void ShutDown();
    void ShutDownInLoop();
    EventLoop* Loop() const {return loop_;};
private:
    enum STATE
    {
        CONNECTED=0,
        DISCONNECTED,
        DISCONNECTING,
        CONNECTING
    };
    std::unique_ptr<Channel> channel_;
    EventLoop *loop_;
    MessageCallback message_cb_;
    CloseCallback close_cb_;
    Buffer read_buffer_;
    Buffer write_buffer_;
    std::atomic<int> state_;

    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();
};