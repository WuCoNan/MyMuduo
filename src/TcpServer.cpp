#include "TcpServer.hpp"
#include "Acceptor.hpp"
#include "EventLoopThreadPool.hpp"
#include "TcpConnection.hpp"
#include "EventLoop.hpp"

TcpServer::TcpServer(EventLoop* mainloop)
                    :loop_(mainloop)
                    ,acceptor_(new Acceptor{mainloop})
                    ,pool_(new EventLoopThreadPool())
{
    acceptor_->SetNewConnCallback(std::bind(&TcpServer::NewConnection,this,std::placeholders::_1,std::placeholders::_2));
}

void TcpServer::SetSubLoopNum(int num)
{
    pool_->SetNum(num);
}

void TcpServer::Start()
{
    acceptor_->Listen();
    pool_->Start();
}

void TcpServer::NewConnection(int connfd,sockaddr peeraddr)
{
    auto subloop=pool_->GetNextLoop();

    auto conn=std::make_shared<TcpConnection>(subloop,connfd);
    conns_.emplace(conn);
    conn->SetMessageCallback(message_cb_);
    conn->SetCloseCallback(std::bind(&TcpServer::RemoveConnection,this,std::placeholders::_1));

    subloop->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished,conn));
}

void TcpServer::RemoveConnection(const std::shared_ptr<TcpConnection>& conn)
{
    loop_->RunInLoop(std::bind(&TcpServer::RemoveConnctionInLoop,this,conn));
}

void TcpServer::RemoveConnctionInLoop(const std::shared_ptr<TcpConnection>& conn)
{
    conns_.erase(conn);
    auto subloop=conn->Loop();
    subloop->RunInLoop(std::bind(&TcpConnection::ConnectionDestroyed,conn));
}

void TcpServer::SetMessageCallback(MessageCallback cb)
{
    message_cb_=cb;
}