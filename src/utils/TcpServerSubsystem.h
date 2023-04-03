#ifndef __TCP_SERVER_SUBSYSTEM_H__
#define __TCP_SERVER_SUBSYSTEM_H__

#include <set>

#include "Subsystem.h"

class TcpServerSubsystem : public Subsystem<TcpServerSubsystem> {
 private:
    int _sockfd = 0;
    int _connfd = 0;

 public:

    void Init(uint16_t port);
    void Teardown() override;
};

#endif  // __TCP_SERVER_SUBSYSTEM_H__
