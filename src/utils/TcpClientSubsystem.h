#ifndef __TCP_CLIENT_SUBSYSTEM_H__
#define __TCP_CLIENT_SUBSYSTEM_H__

#include "List.h"
#include "Subsystem.h"
#include "SoftwareBusSubsystem.h"

#include <mutex>

#include <set>
#include <string>

class TcpClientSubsystem : public Subsystem<TcpClientSubsystem> {
 private:
    int _sockfd = 0;
    std::string _endpoint;

    SoftwareBusSubsystem *_software_bus;

    static constexpr size_t RX_BUFSIZE = 2048;

 public:
    List<TcpClientSubsystem>::Node _node;

    void Init(const std::string &ip_addr, uint16_t port, const std::string &endpoint, SoftwareBusSubsystem &software_bus);
    void InitFromServer(int sockfd, SoftwareBusSubsystem &software_bus);

    void Teardown() override;

    void Send(const void *data, size_t length);
    void ThreadFunction() override;
};

#endif  // __TCP_CLIENT_SUBSYSTEM_H__
