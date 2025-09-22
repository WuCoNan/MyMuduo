#pragma once
#include "Noncopyable.hpp"
#include <functional>
#include <memory>

class EventLoop;
class TcpConnection;
class Channel : public Noncopyable
{
public:
    using EventCallback=std::function<void()>;

    Channel(int fd, EventLoop *loop);
    ~Channel();
    void HandleEvent();
    void SetRevents(int revents);
    bool IsReadEvent() const;
    bool IsWriteEvent() const;
    bool IsNoneEvent() const;
    void EnableRead();
    void EnableWrite();
    void DisableRead();
    void DisableWrite();
    void DisableAll();
    int GetFd() const;
    int GetEvents() const;
    void SetReadCallback(EventCallback);
    void SetWriteCallback(EventCallback);
    void SetErrorCallback(EventCallback);
    void SetCloseCallback(EventCallback);

    void Tie(const std::shared_ptr<TcpConnection>& obj);
    void Remove();

private:
    int revents_;
    int events_;
    int fd_;
    bool tied_;
    EventLoop *loop_;
    std::weak_ptr<TcpConnection> tie_;

    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback error_callback_;
    EventCallback close_callback_;
};