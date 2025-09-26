#include "TcpConnection.hpp"
#include "EventLoop.hpp"
#include "Logger.hpp"
TcpConnection::TcpConnection(EventLoop* loop,int fd)
                            :loop_(loop)
                            ,channel_(new Channel(fd,loop))
                            ,state_(CONNECTING)
{
    channel_->SetReadCallback(std::bind(&TcpConnection::HandleRead,this));
    channel_->SetCloseCallback(std::bind(&TcpConnection::HandleClose,this));
}
TcpConnection::~TcpConnection()
{
    
}
void TcpConnection::ConnectionEstablished()
{
    channel_->Tie(shared_from_this());
    channel_->EnableRead();
    state_=CONNECTED;

    //loop_->RunAfter([this](){ShutDown();},10);

    LOG_INFO("server   gets   a    new    connnection   with   Fd   %d\n",channel_->GetFd());
}

void TcpConnection::ConnectionDestroyed()
{
    channel_->Remove();
    state_=DISCONNECTED;
    LOG_INFO("server   removes   a   connnection   with   Fd   %d\n",channel_->GetFd());
}
void TcpConnection::HandleRead()
{
    int save_errno;

    ssize_t n=read_buffer_.ReadFd(channel_->GetFd(),&save_errno);

    if(n>0&&message_cb_)
        message_cb_(shared_from_this(),&read_buffer_);
    else if(n==0)
        HandleClose();
    else if(save_errno!=EAGAIN&&save_errno!=EWOULDBLOCK)
        HandleError();

}

void TcpConnection::HandleClose()
{
    if(state_==DISCONNECTED)
        return;

    state_=DISCONNECTED;
    channel_->DisableAll();
    close_cb_(shared_from_this());
}

void TcpConnection::HandleError()
{
    std::cout<<"ERROR!!!"<<std::endl;
    exit(1);
}

void TcpConnection::HandleWrite()
{
    int save_errno;
    ssize_t n=write_buffer_.WriteFd(channel_->GetFd(),&save_errno);

    if(n<=0&&save_errno!=EAGAIN&&save_errno!=EWOULDBLOCK)
        HandleError();
    else if(write_buffer_.ReadableBytes()==0)
    {
        channel_->DisableWrite();
        if(state_==DISCONNECTING)
            ::shutdown(channel_->GetFd(),SHUT_WR);
    }
}

void TcpConnection::Send(const char* data,size_t len)
{
    if(state_!=CONNECTED)
        return;
    if(loop_->IsInLoopThread())
        SendInLoop(data,len);
    else
        loop_->RunInLoop(std::bind(&TcpConnection::SendInLoop,shared_from_this(),data,len));
}

void TcpConnection::SendInLoop(const char* data,size_t len)
{
    size_t remaining=len;
    
    if(!channel_->IsWriteEvent())
    {
        ssize_t n=::write(channel_->GetFd(),data,len);

        if(n<=0)
        {
            if(errno!=EAGAIN&&errno!=EWOULDBLOCK)
                HandleError();
        }
        else
        {
            remaining-=n;
        }
    }

    if(remaining>0)
    {
        write_buffer_.Append(data+remaining,remaining);
        if(!channel_->IsWriteEvent())
            channel_->EnableWrite();
    }
}

void TcpConnection::ShutDown()
{
    state_=DISCONNECTING;
    loop_->RunInLoop(std::bind(&TcpConnection::ShutDownInLoop,shared_from_this()));
}

void TcpConnection::ShutDownInLoop()
{
    if(!channel_->IsWriteEvent())
    {
        ::shutdown(channel_->GetFd(),SHUT_WR);
    }
}

void TcpConnection::SetMessageCallback(MessageCallback cb)
{
    message_cb_=cb;
}

void TcpConnection::SetCloseCallback(CloseCallback cb)
{
    close_cb_=cb;
}