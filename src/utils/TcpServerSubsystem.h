#ifndef __TCP_SERVER_SUBSYSTEM_H__
#define __TCP_SERVER_SUBSYSTEM_H__

#include <map>
#include <array>
#include <vector>

#include <poll.h>
#include <unistd.h>

#include "List.h"
#include "Subsystem.h"

#include "TcpMessages.h"

class TcpServerSubsystem : public Subsystem<TcpServerSubsystem> {
 private:
    // Lol pipes are hard to make
    class Tunnel {
     private:
        std::array<int, 2> _file_descriptors;

     public:
        const std::string _name;
        List<Tunnel>::Node _node;

        bool ForwardFrom(int fd_from, TcpMessage *message) {
            if (_file_descriptors[0] != fd_from && _file_descriptors[1] != fd_from) {
                return false;
            }

            int fd_to = _file_descriptors[0];
            if (_file_descriptors[0] == fd_from) {
                fd_to = _file_descriptors[1];
            }

            size_t nsent = 0;
            while (nsent < message->_size) {
                nsent += write(fd_to, ((uint8_t *)message) + nsent, message->_size - nsent);
            }

            return true;
        }
    };

    class TunnelManager {
     private:
        List<Tunnel> _tunnels;
        std::map<std::string, Tunnel *> _by_name;
        std::map<int, Tunnel *> _by_fd;

     public:
        void Register(int fd, const std::string &name);
        void Unregister(int fd);
    };

    int _sockfd = 0;
    std::string _endpoint_prefix;

    std::vector<struct pollfd> _connection_polls;
    // Do I really need a collection of TCP clients for 1:1 connection? Something I could do is potentially do a READ
    // and route to the right endpoint if the message looks right

    // Manage lots of 1:1 TCP tunnels

    // Can't target a specific FD, since that's process specific
    // CAN potentially target specific endpoints on software bus
    // Packets need 2 types of operations -> (1) FORWARD, (2) REGISTER
    // Clients will REGISTER for server via endpoint ID

    // Index of int (fd) to pipe object

    void RegisterSocketFd(int fd);

    static constexpr size_t BACKLOG_ENTRIES = 4;

 public:
    void Init(uint16_t port);

    void Teardown() override;
    void ThreadFunction() override;
};

#endif  // __TCP_SERVER_SUBSYSTEM_H__
