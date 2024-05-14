
#ifdef __linux__
#include <boost/asio/buffer.hpp>
#include <condition_variable>
#include "epoll_tcp_server.h"
#include "logger.h"

namespace utils {
namespace epoll {
namespace tcp {
_TcpClient::_TcpClient(int fd, int epoll_fd, struct sockaddr_in sa) : _fd(fd), _epoll_fd(epoll_fd), _sa(sa) {
    event = std::make_unique<struct epoll_event>();
    event->events = EPOLLIN | EPOLLERR | EPOLLRDHUP;
    event->data.ptr = this;
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, event.get());
}
_TcpClient::~_TcpClient() {
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _fd, nullptr);
    close(_fd);
};

_WorkThread::_WorkThread(unsigned short _port) {
    struct sockaddr_in server_addr{
        .sin_family = AF_INET,
        .sin_port = htons(_port),
        .sin_addr = INADDR_ANY,
    };
    int optval = 1;
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0) {
        throw std::runtime_error("Failed to create socke");
    }

    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (bind(_server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Bind failed");
    }
    if (listen(_server_fd, 10) < 0) {
        throw std::runtime_error("Listen failed");
    }
    _epoll_fd = epoll_create1(0);
    _thread = std::thread([epoll_fd = _epoll_fd, server_fd = _server_fd, clients = _clients]() {
        struct epoll_event event = {
            .events = EPOLLIN,
            .data {.fd = server_fd}
        };
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);
        struct epoll_event events[MAX_EVENTS];
        while (true) {
            int num = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);
            if (num < 0) {
                if (errno == EINTR) continue;
                break;
            }
            for (int i = 0; i < num; i++) {
                if (events[i].data.fd == server_fd) {
                    if (events[i].events & EPOLLIN) {
                        struct sockaddr_in sa;
                        socklen_t len = sizeof(sa);
                        int client_fd = accept(events[i].data.fd, (struct sockaddr *) &sa, &len);
                        std::unique_ptr<_TcpClient> client = std::make_unique<_TcpClient>(client_fd, epoll_fd, sa);
                        clients->insert({client_fd, std::move(client)});

                    }
                } else {
                    if (events[i].events & EPOLLERR) goto close;
                    if (events[i].events & EPOLLRDHUP) goto close;
                    if (events[i].events & EPOLLIN) {
                        char buffer[1024] = {0};
                        _TcpClient *client = static_cast<_TcpClient *>(events[i].data.ptr);
                        int bytes_received = recv(client->_fd, buffer, 1024, 0);
                        if (bytes_received <= 0) {
                            goto close;
                        }
                        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!\r\n";
                        send(client->_fd, response, strlen(response), 0);
                    }
                    // if (events[i].events & EPOLLOUT) {
                    //     std::cout << std::to_string(events[i].data.fd) << ": " << "EPOLLOUT" << std::endl;
                    // }
                    continue;
                    close:
                    _TcpClient *client = static_cast<_TcpClient *>(events[i].data.ptr);
                    clients->erase(client->_fd);
                }
            }
        }
    });
}

_WorkThread::~_WorkThread() {
    close(_epoll_fd);
    close(_server_fd);
    if (_thread.joinable()) {
        _thread.join();
    }
}

EpollTCPServer::EpollTCPServer(
    std::function<void(int)> on_connected_handle,
    std::function<void(int, const char *, size_t)> on_message_handle,
    std::function<void(int)> on_disconnected_handle) {

    this->on_connected_handle = on_connected_handle;
    this->on_message_handle = on_message_handle;
    this->on_disconnected_handle = on_disconnected_handle;
}
EpollTCPServer::~EpollTCPServer() {

}

int EpollTCPServer::start(short port) {
    for (int i = 0; i < 10; i++) {
        _threads.push_back(std::move(_WorkThread(port)));
    }
    log_info("TCP Server [{}] 已启动", port);
    return 0;
}

void EpollTCPServer::stop() {
    _threads.clear();
}

// static std::string ByteToHexString(const char* data, size_t length) {
//     std::stringstream ss;
//     ss << std::hex << std::setfill('0');
//     for (size_t i = 0; i < length; ++i) {
//         ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(data[i]));
//     }
//     return ss.str();
// }

EpollTCPServer &&EpollTCPServer::getTestInstance() {
    static EpollTCPServer tcpd(
        [](int socket) {},
        [](int socket, const char *data, size_t size) {
            // log_info(socket.remote_endpoint().address().to_string()
            //     << ":"
            //     << socket.remote_endpoint().port() 
            //     << " 收到数据: " << ByteToHexString(data,size));
        },
        [](int socket) {}
    );
    tcpd.start(10080);
    return std::move(tcpd);
}
}}}
#endif