#pragma once
#include "Noncopyable.hpp"
#include <vector>
#include <string>
class Buffer : public Noncopyable
{
public:
    static constexpr size_t initial_size_ = 1024;
    explicit Buffer(size_t initial_size = initial_size_)
        : buffer_(initial_size), reader_index_(0), writer_index_(0)
    {
    }
    void Append(const char *data, size_t len);
    void EnsureWritableBytes(size_t len);
    size_t WritableBytes() const { return buffer_.size() - writer_index_; }
    size_t ReadableBytes() const { return writer_index_ - reader_index_; }
    size_t FreeBytes() const { return buffer_.size() - ReadableBytes(); }
    ssize_t ReadFd(int fd, int *save_errno);
    ssize_t WriteFd(int fd,int *save_errno);
    std::string RetriveAsString(size_t len);
    std::string RetriveAllAsString();
    void Retrieve(size_t len) { reader_index_ += std::min(ReadableBytes(), len); }

    char *BeginRead() { return Begin()+reader_index_; }
    const char *BeginRead() const { return Begin()+reader_index_; }
    char *BeginWrite() { return Begin()+writer_index_; }
    const char *BeginWrite() const { return Begin()+writer_index_; }
    char *Begin() { return &*buffer_.begin(); }
    const char *Begin() const { return &*buffer_.begin(); }

private:
    std::vector<char> buffer_;
    size_t reader_index_;
    size_t writer_index_;

    void MakeSpace(size_t len);
    
    void RetriveAll() { reader_index_ += ReadableBytes(); }
};