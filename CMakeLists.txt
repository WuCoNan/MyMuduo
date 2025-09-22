cmake_minimum_required(VERSION 3.10)
project(Muduo)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_BUILD_TYPE "Release")

# 添加可执行文件 - 使用明确的目标名称
add_executable(muduo_server
    test.cpp
    src/Acceptor.cpp
    src/Buffer.cpp
    src/Channel.cpp
    src/EventLoop.cpp
    src/EventLoopThread.cpp
    src/EventLoopThreadPool.cpp
    src/Poller.cpp
    src/TcpConnection.cpp
    src/TcpServer.cpp
    src/Logger.cpp
)

# 链接 pthread 库
target_link_libraries(muduo_server pthread)