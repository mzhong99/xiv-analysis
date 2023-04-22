#include "TcpServerSubsystem.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <spdlog/spdlog.h>

void TcpServerSubsystem::RegisterSocketFd(int fd) {
    struct pollfd poller = {
        .fd = fd,
        .events = POLLIN | POLLPRI | POLLRDHUP
    };
}

void TcpServerSubsystem::Init(uint16_t port) {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (_sockfd == -1) {
        spdlog::error("Could not create socket (rc={})", _sockfd);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int rc;
    if ((rc = bind(_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        spdlog::error("Could not bind socket (rc={})", rc);
    }

    RegisterSocketFd(_sockfd);

    if ((rc = listen(_sockfd, TcpServerSubsystem::BACKLOG_ENTRIES)) != 0) {
        spdlog::error("Could not listen to socket (rc={})", rc);
    }

    spdlog::info("Listen on fd={}", _sockfd);

    StartWorkerThread();
    Subsystem<TcpServerSubsystem>::Init();
}

void TcpServerSubsystem::ThreadFunction() {
    while (true) {
        if (poll(_connection_polls.data(), _connection_polls.size(), -1) == 0) {
            continue;
        }

        for (const struct pollfd &poll_result : _connection_polls) {
            if (poll_result.revents == 0) {
                continue;
            }

            spdlog::info("FD {} -> {:b}, {:b}", poll_result.fd, poll_results.events, poll_results.revents);

            if (poll_result.fd == _sockfd) {
                // either accept or close
                // Register a new sockfd
                //
                // Need some way to bind a socket to specific endpoints
                // if close, swap with back and pop1
            } else {
                // read stuff, post over endpoint prefix for now
                // TODO: Need some strategic way to bind a reader/writer from a TCP client - maybe alternative constructor
                //       that never opens a socket connection and just reuses the same interface?
            }
        }
    }
}

void TcpServerSubsystem::Teardown() {
    Subsystem<TcpServerSubsystem>::Teardown();
}
