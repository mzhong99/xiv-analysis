#include "NetworkMonitor.h"

#include <spdlog/spdlog.h>
#include <subprocess.hpp>

#include <linux/if_packet.h>

#include <exception>
#include <sstream>

NetworkMonitor::NetworkMonitor(const std::string &ip_filter, SoftwareBusSubsystem &software_bus) {
    _ip_filter = ip_filter;
    _software_bus = &software_bus;

    int rc;
    char errbuf[PCAP_ERRBUF_SIZE];

    if ((_pcap = pcap_create("any", errbuf)) == NULL) {
        throw std::runtime_error(fmt::format("Could not create pcap object: {}", errbuf));
    }

    pcap_set_immediate_mode(_pcap, 1);
    pcap_set_timeout(_pcap, 10);

    if ((rc = pcap_activate(_pcap)) != 0) {
        throw std::runtime_error(fmt::format("Could not activate pcap object: {}", rc));
    }

    struct bpf_program filter_program;
    std::string filter = fmt::format("ip host {}", ip_filter);
    if ((rc = pcap_compile(_pcap, &filter_program, filter.c_str(), 0, 0)) != 0) {
        spdlog::info("Could not compile PCAP filter: {}", rc);
    }
    pcap_setfilter(_pcap, &filter_program);
    pcap_setnonblock(_pcap, 1, errbuf);

    StartWorkerThread();
}

NetworkMonitor::~NetworkMonitor() {
    if (_pcap != NULL) {
        pcap_close(_pcap);
        _pcap = NULL;
    }
}

bool NetworkMonitor::HasServerConnection() {
    auto ss_result = subprocess::pipeline("ss -nap", "grep ESTAB", fmt::format("grep {}", _ip_filter));
    return ss_result.length > 0;
}

std::string NetworkMonitor::FindIpHeaderMatch(const struct ip *ip_header) {
    const std::string ip_src = inet_ntoa(ip_header->ip_src);
    const std::string ip_dst = inet_ntoa(ip_header->ip_dst);

    if (ip_src.rfind(_ip_filter) == 0) {
        return ip_src;
    } else if (ip_dst.rfind(_ip_filter) == 0) {
        return ip_dst;
    } else {
        return "";
    }
}

const struct ip *NetworkMonitor::FindIpHeader(const void *raw_packet, size_t size)
{
    if (NetworkMonitor::FindIpHeaderMatch((const struct ip *)raw_packet) != "") {
        return (const struct ip *)raw_packet;
    }

    raw_packet = (const void *)(((const uint8_t *)raw_packet) + NetworkMonitor::ANY_HEADER_SIZE);
    if (NetworkMonitor::FindIpHeaderMatch((const struct ip *)raw_packet) != "") {
        return (const struct ip *)raw_packet;
    }

    return NULL;
}

const struct tcphdr *NetworkMonitor::FindTcpHeader(const struct ip *ip_packet) {
    if (ip_packet->ip_p != IPPROTO_TCP) {
        return NULL;
    }

    return (const struct tcphdr *)((const uint8_t *)(ip_packet) + ip_packet->ip_hl);
}

const void *NetworkMonitor::FindTcpPayload(const struct tcphdr *tcp_packet) {
    return (const void *)(((const uint8_t*)tcp_packet) + (sizeof(uint32_t) * tcp_packet->th_off));
}

void NetworkMonitor::UpdateServerConnectionStatus() {
    bool server_alive = HasServerConnection();
    if (_connected && !server_alive) {
        spdlog::info("Disconnected from {}", _ip_match);
    }

    if (!_connected && server_alive) {
        spdlog::info("Connected to {}", _ip_match);
    }
    _connected = server_alive;
}

void NetworkMonitor::ProcessPcapPacket(const struct pcap_pkthdr *header, const uint8_t *raw_packet) {
    size_t raw_packet_size = header->caplen;
    if (raw_packet == NULL) {
        return;
    }

    if (raw_packet_size < NetworkMonitor::REQUIRED_HEADER_SIZE) {
        spdlog::info("Packet size {} is too small to parse (required size {})",
            raw_packet_size, NetworkMonitor::REQUIRED_HEADER_SIZE);
        return;
    }

    const struct ip *ip_header = FindIpHeader(raw_packet, raw_packet_size);
    if (ip_header == NULL) {
        return;
    }
    _ip_match = FindIpHeaderMatch(ip_header);

    const struct tcphdr *tcp_header = FindTcpHeader(ip_header);
    if (tcp_header == NULL) {
        return;
    }

    const void *payload = FindTcpPayload(tcp_header);
    size_t payload_size = raw_packet_size - ((size_t)payload - (size_t)raw_packet);
    _software_bus->Publish("/net/tcp_payloads", payload, payload_size);

    spdlog::info("Got payload of size {}, last byte {}", payload_size, +(((const uint8_t *)payload)[raw_packet_size - 1]));
}

void NetworkMonitor::ThreadFunction() {
    while (_running) {
        struct pcap_pkthdr header;
        const uint8_t *raw_packet = pcap_next(_pcap, &header);
        ProcessPcapPacket(&header, raw_packet);
        UpdateServerConnectionStatus();
    }
}
