#include "TcpClientSubsystem.h"
#include <spdlog/spdlog.h>

#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

void TcpClientSubsystem::ThreadFunction() {
    struct pollfd pollfd;

    uint8_t rx_buffer[TcpClientSubsystem::RX_BUFSIZE];
    while (_running) {

        memset(rx_buffer, 0, sizeof(rx_buffer));
        ssize_t rx_count = read(_sockfd, rx_buffer, TcpClientSubsystem::RX_BUFSIZE);
        if (rx_count == 0) {
            break;
        }
    }
}

void TcpClientSubsystem::Init(std::string ip_addr, uint16_t port, SoftwareBusSubsystem &software_bus) {
    _software_bus = &software_bus;

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        spdlog::error("Could not create socket (rc={})", _sockfd);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    server_addr.sin_port = htons(port);

    int rc = connect(_sockfd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
    if (rc != 0) {
        spdlog::error("Could not create socket (rc={})", _sockfd);
    }

    StartWorkerThread();
    Subsystem<TcpClientSubsystem>::Init();
}

void TcpClientSubsystem::Teardown() {
    if (_sockfd > 0) {
        close(_sockfd);
    }
    Subsystem<TcpClientSubsystem>::Teardown();
}
