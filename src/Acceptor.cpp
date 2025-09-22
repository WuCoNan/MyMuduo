#include "Acceptor.hpp"
#include "EventLoop.hpp"

Acceptor::Acceptor(EventLoop* loop)
                  :loop_(loop)
                  ,accept_fd_(CreateAcceptFd())
                  ,accept_channel_(new Channel{accept_fd_,loop})
{
    accept_channel_->SetReadCallback(std::bind(&Acceptor::HandleRead,this));
}

void Acceptor::SetNewConnCallback(NewConnectionCallback newconn_callback)
{
    newconn_callback_=newconn_callback;
}
int Acceptor::CreateAcceptFd()
{
    return ::socket(AF_INET,SOCK_CLOEXEC|SOCK_NONBLOCK|SOCK_STREAM,IPPROTO_TCP);
}

void Acceptor::Listen()
{
    sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(8080);

    ::bind(accept_fd_,(sockaddr*)&addr,sizeof(addr));
    ::listen(accept_fd_,max_accept_once_);
    
    accept_channel_->EnableRead();
}

void Acceptor::HandleRead()
{
    sockaddr peeraddr;
    socklen_t addrlen;
    int connfd=::accept4(accept_fd_,&peeraddr,&addrlen,SOCK_CLOEXEC|SOCK_NONBLOCK);

    auto save_errno=errno;

    if(connfd<0)
        exit(-1);
    
    newconn_callback_(connfd,peeraddr);
}