#include "Buffer.hpp"
#include <sys/uio.h>
#include <errno.h>
#include <unistd.h>
ssize_t Buffer::ReadFd(int fd, int *save_errno)
{
    char extra_buffer[65536];

    struct iovec iov[2];
    iov[0].iov_base = BeginWrite();
    iov[0].iov_len = WritableBytes();
    iov[1].iov_base = extra_buffer;
    iov[1].iov_len = sizeof(extra_buffer);

    int iov_cnt = WritableBytes() < sizeof(extra_buffer) ? 2 : 1;

    ssize_t n = ::readv(fd, iov, iov_cnt);

    if (n <= 0)
        *save_errno = errno;
    else if (n <= WritableBytes())
        writer_index_ += n;
    else
    {
        writer_index_ = buffer_.size();
        Append(extra_buffer, n - WritableBytes());
    }

    return n;
}

ssize_t Buffer::WriteFd(int fd,int *save_errno)
{
    ssize_t n=::write(fd,BeginRead(),ReadableBytes());

    if(n<=0)
        *save_errno=errno;
    else
        reader_index_+=n;

    return n;
}

void Buffer::Append(const char *data, size_t len)
{
    EnsureWritableBytes(len);

    std::copy(data, data + len, BeginWrite());

    writer_index_ += len;
}
void Buffer::MakeSpace(size_t len)
{
    std::copy(BeginRead(), BeginRead() + ReadableBytes(), Begin());
    writer_index_ -= reader_index_;
    reader_index_ = 0;

    if (WritableBytes() < len)
        buffer_.resize(buffer_.size() + len - WritableBytes());
}
void Buffer::EnsureWritableBytes(size_t len)
{
    if (WritableBytes() >= len)
        return;
    MakeSpace(len);
}
std::string Buffer::RetriveAsString(size_t len)
{
    std::string ret(BeginRead(), BeginRead() + std::min(len, ReadableBytes()));
    Retrieve(len);
    return ret;
}
std::string Buffer::RetriveAllAsString()
{
    return RetriveAsString(ReadableBytes());
}