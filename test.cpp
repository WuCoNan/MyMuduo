#include "TcpServer.hpp"
#include "Buffer.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"
#include "Logger.hpp"
#include <iostream>
void MessageFunc(const std::shared_ptr<TcpConnection>& conn, Buffer *buffer)
{
    auto str = buffer->RetriveAllAsString();
    //std::cout << "server    has    received : " << str << std::endl;
    LOG_INFO("server    has    received : %s\n",str.data());
    conn->Send(str.data(),str.size());
    
    //conn->ShutDown();
}

void TestOn()
{
    EventLoop main_loop;
    TcpServer server(&main_loop);
    server.SetSubLoopNum(3);
    server.SetMessageCallback(MessageFunc);

    server.Start();
    main_loop.Loop();
}

int main()
{

    TestOn();

    return 0;
}