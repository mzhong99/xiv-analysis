#ifndef __TCP_CLIENT_SUBSYSTEM_H__
#define __TCP_CLIENT_SUBSYSTEM_H__

#include "Subsystem.h"

#include <mutex>

#include <vector>
#include <string>

class TcpClientSubsystem : public Subsystem<TcpClientSubsystem> {
 private:
    int _sockfd = 0;

    SoftwareBusSubsystem *_software_bus;

    std::mutex _target_endpoints_lock;
    std::vector<std::string> _target_endpoints;

    static constexpr size_t RX_BUFSIZE = 2048;

 public:
    void Init(std::string ip_addr, uint16_t port, SoftwareBusSubsystem &software_bus);
    void Teardown() override;

    void Send(const void *data, size_t length);
    void BindToEndpoint(const std::string &endpoint);

    void ThreadFunction() override;
};

#endif  // __TCP_CLIENT_SUBSYSTEM_H__
