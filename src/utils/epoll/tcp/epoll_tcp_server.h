#ifndef __EPOLL_TCP_SERVER_H_
#define __EPOLL_TCP_SERVER_H_

#ifdef __linux__
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "logger.h"

#define MAX_EVENTS 10
#define NUM_THREADS 1

namespace utils {
namespace epoll {
namespace tcp {

class _TcpClient {
private:
    std::unique_ptr<struct epoll_event> event;
public:
    struct sockaddr_in _sa;
    int _epoll_fd;
    int _fd;
    _TcpClient(int fd, int epoll_fd, struct sockaddr_in sa);
    ~_TcpClient();
};

class _WorkThread {
private:
    int _epoll_fd;
    int _server_fd;
    std::thread _thread;
    std::shared_ptr<std::unordered_map<int, std::unique_ptr<_TcpClient>>>
        _clients = std::make_shared<std::unordered_map<int, std::unique_ptr<_TcpClient>>>();
public:
    _WorkThread(unsigned short _port);
    _WorkThread(_WorkThread &&other) {
        _thread = std::move(other._thread);
        _clients = std::move(other._clients);
        _epoll_fd = other._epoll_fd;
        _server_fd = other._server_fd;
        other._epoll_fd = 0;
        other._server_fd = 0;
    }
    _WorkThread &operator=(_WorkThread &&other) {
        if (this != &other) {
            _thread = std::move(other._thread);
            _clients = std::move(other._clients);
            _epoll_fd = other._epoll_fd;
            _server_fd = other._server_fd;
            other._epoll_fd = 0;
            other._server_fd = 0;
        }
        return *this;
    }
    ~_WorkThread();
};

class EpollTCPServer {
private:
    int _server_fd;
    std::vector<_WorkThread> _threads;
    std::function<void(int)> on_connected_handle;
    std::function<void(int, const char *, size_t)> on_message_handle;
    std::function<void(int)> on_disconnected_handle;
    std::unique_ptr<std::thread> thread;

public:
    EpollTCPServer(EpollTCPServer &&other) {
        on_connected_handle = std::move(other.on_connected_handle);
        on_message_handle = std::move(other.on_message_handle);
        on_disconnected_handle = std::move(other.on_disconnected_handle);
        thread = std::move(other.thread);
    }
    EpollTCPServer &operator=(EpollTCPServer &&other) {
        if (this != &other) {
            on_connected_handle = std::move(other.on_connected_handle);
            on_message_handle = std::move(other.on_message_handle);
            on_disconnected_handle = std::move(other.on_disconnected_handle);
            thread = std::move(other.thread);
        }
        return *this;
    }

    EpollTCPServer(
        std::function<void(int)> on_connected_handle,
        std::function<void(int, const char *, size_t)> on_message_handle,
        std::function<void(int)> on_disconnected_handle);

    ~EpollTCPServer();

    int start(short port);
    void stop();
    void session_handler(int socket);
    static EpollTCPServer &&getTestInstance();
};
}
}
}
#endif
#endif