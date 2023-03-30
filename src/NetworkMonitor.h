#ifndef __NETWORK_MONITOR_H__
#define __NETWORK_MONITOR_H__

#include "Subsystem.h"
#include "SoftwareBusSubsystem.h"

#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <string>

class NetworkMonitor : public Subsystem<NetworkMonitor> {
 private:
    SoftwareBusSubsystem *_software_bus;
    std::string _ip_filter;

    pcap_t *_pcap = NULL;

    static constexpr size_t ANY_HEADER_SIZE = 16u;
    static constexpr size_t REQUIRED_HEADER_SIZE = sizeof(struct ip) + sizeof(struct tcphdr);

    bool IpHeaderMatches(const struct ip *ip_header);
    const struct ip *FindIpHeader(const void *raw_packet, size_t size);
    const void *FindTcpHeader(const void *ip_packet, size_t size);

    void *FindTcpPayload(const void *tcp_packet, size_t size);
    size_t FindTcpPayloadSize(const struct tcphdr *tcp_packet);

 public:
    static constexpr std::string NA_DATA_CENTER_ADDRESS = "204.2.229";
    static constexpr std::string EU_DATA_CENTER_ADDRESS = "80.239.145";

    NetworkMonitor(const std::string &ip_filter, SoftwareBusSubsystem &software_bus);
    ~NetworkMonitor();

    void ThreadFunction() override;
    bool HasServerConnection();
};

#endif  // __NETWORK_MONITOR_H__
