// server_test.cpp
// 编译命令: g++ -std=c++11 -o server_test server_test.cpp TimerQueue.cpp EventLoop.cpp TcpServer.cpp TcpConnection.cpp Buffer.cpp Channel.cpp EventLoopThreadPool.cpp Acceptor.cpp EventLoopThread.cpp Logger.cpp TimeStamp.cpp Poller.cpp Timer.cpp -lpthread
// 运行: ./server_test
// 注意: 端口 8080 来自 Acceptor.cpp 中的硬编码。如果需要修改，请调整 Acceptor::Listen()。
// SetSubLoopNum 设置为 8 个工作线程，以处理高并发连接。

#include <iostream>
#include <memory>
#include "TcpServer.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"
#include "Buffer.hpp"
#include "TcpConnection.hpp"

void onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf) {
    // 简单回显: 将接收到的数据发送回去
    size_t len = buf->ReadableBytes();
    if (len > 0) 
    {
        auto str=buf->RetriveAllAsString();
        conn->Send(str.data(), len);
    }
}

int main() {
    EventLoop loop;
    TcpServer server(&loop);
    server.SetMessageCallback(onMessage);
    server.SetSubLoopNum(8);  // 使用 8 个工作线程处理并发连接
    server.Start();

    LOG_INFO("回显服务器已在端口 8080 启动\n");
    loop.Loop();
    return 0;
}