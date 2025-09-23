#pragma once
#include <functional>
#include <memory>

class TcpConnection;
class Buffer;

using MessageCallback = std::function<void(const std::shared_ptr<TcpConnection>&, Buffer *)>;
using CloseCallback=std::function<void(const std::shared_ptr<TcpConnection>&)>;
using TimerCallback=std::function<void(void)>;