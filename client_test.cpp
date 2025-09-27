// client_test.cpp
// 编译命令: g++ -std=c++11 -o client_test client_test.cpp TcpConnection.cpp EventLoop.cpp TimerQueue.cpp Buffer.cpp Channel.cpp Poller.cpp TimeStamp.cpp Timer.cpp Logger.cpp -lpthread
// 运行: ./client_test（先启动服务器）
// 调整 NUM_THREADS、MESSAGE_SIZE、NUM_MESSAGES 以增加压力。
// 每个线程使用独立 EventLoop 和 TcpConnection，发送消息并等待回显。
// 输出每个线程和整体吞吐量（消息/秒）。

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <memory>
#include <cstring>  // 用于 memset
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>  // 用于 fcntl (非阻塞)
#include <errno.h>
#include "EventLoop.hpp"
#include "TcpConnection.hpp"
#include "Buffer.hpp"
#include "Logger.hpp"

const int NUM_THREADS = 50;       // 并发连接/线程数
const size_t MESSAGE_SIZE = 1024; // 每个消息的负载大小（1KB）
const int NUM_MESSAGES = 10000;   // 每个连接发送的消息数（总计 50 * 10k = 500k 请求）
const std::string HOST = "127.0.0.1";
const int PORT = 8080;

std::atomic<long long> total_requests{0};
std::atomic<long long> total_responses{0};
std::chrono::steady_clock::time_point start_time;

// 简单 InetAddress 实现（基于 sockaddr_in，用于 connect）
class InetAddress {
public:
    InetAddress(const std::string& ip, uint16_t port) {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
    }
    const struct sockaddr* getSockAddr() const {
        return reinterpret_cast<const struct sockaddr*>(&addr_);
    }
    socklen_t size() const { return sizeof(addr_); }
private:
    struct sockaddr_in addr_;
};

class StressClient {
public:
    StressClient(EventLoop* loop, const InetAddress& server_addr)
        : loop_(loop), conn_(nullptr), sent_count_(0), received_count_(0) {
        // 创建非阻塞 socket
        int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
        if (fd < 0) {
            LOG_ERROR("创建 socket 失败: %s\n", strerror(errno));
            return;
        }
        // 发起非阻塞 connect
        int ret = ::connect(fd, server_addr.getSockAddr(), server_addr.size());
        if (ret < 0 && errno != EINPROGRESS) {
            LOG_ERROR("连接失败: %s\n", strerror(errno));
            ::close(fd);
            return;
        }
        // 创建 TcpConnection
        conn_ = std::make_shared<TcpConnection>(loop, fd);
        conn_->SetMessageCallback(std::bind(&StressClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));
        conn_->SetCloseCallback(std::bind(&StressClient::onClose, this));
        // 在 loop 中建立连接
        loop_->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished, conn_));
    }

    void start() {
        sendMessage();
    }
    std::shared_ptr<TcpConnection> conn_;
private:
    void sendMessage() {
        if (sent_count_ >= NUM_MESSAGES || !conn_) return;
        char buf[MESSAGE_SIZE];
        // 用简单模式填充（A-Z 循环）
        memset(buf, 'A' + (sent_count_ % 26), MESSAGE_SIZE);
        conn_->Send(buf, MESSAGE_SIZE);
        ++sent_count_;
        total_requests.fetch_add(1, std::memory_order_relaxed);
    }

    void onMessage(const std::shared_ptr<TcpConnection>&, Buffer* buf) {
        size_t len = buf->ReadableBytes();
        if (len >= MESSAGE_SIZE) {
            buf->Retrieve(MESSAGE_SIZE);  // 消费回显数据
            ++received_count_;
            total_responses.fetch_add(1, std::memory_order_relaxed);
            sendMessage();  // 接收后发送下一个（ping-pong 模式）
            if (received_count_ >= NUM_MESSAGES) {
                auto end = std::chrono::steady_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time).count();
                if (dur > 0) {
                    double tput = (received_count_ * 1000.0) / dur;
                    LOG_INFO("线程 %ld 完成 %d 条消息，用时 %ld ms (%.2f 消息/秒)\n", 
                             std::hash<std::thread::id>{}(std::this_thread::get_id()), received_count_, dur, tput);
                }
                loop_->Quit();
            }
        }
    }

    void onClose() {
        LOG_INFO("连接意外关闭\n");
        loop_->Quit();
    }

    EventLoop* loop_;
    
    int sent_count_;
    int received_count_;
};

// 每个线程的客户端函数
void clientThread() {
    EventLoop loop;
    InetAddress server_addr(HOST, PORT);
    StressClient client(&loop, server_addr);
    if (client.conn_) {  // 只在连接成功时启动
        client.start();
    }
    loop.Loop();
}

int main() {
    start_time = std::chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(clientThread);
    }
    for (auto& t : threads) {
        t.join();
    }
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    double overall_throughput = 0.0;
    if (duration.count() > 0) {
        overall_throughput = static_cast<double>(total_responses.load()) / duration.count() * 1000.0;
    }
    std::cout << "压力测试完成: " << total_requests.load() << " 个请求, " << total_responses.load()
              << " 个响应，用时 " << duration.count() << " ms (整体 " << overall_throughput << " 响应/秒)\n";
    return 0;
}