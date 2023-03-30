#include "NetworkMonitor.h"

#include <spdlog/spdlog.h>
#include <subprocess.hpp>

#include <linux/if_packet.h>

#include <exception>

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
    auto ps_result = subprocess::pipeline("ps -ef", "grep -v grep", "grep ffxiv_dx11.exe");
    if (ps_result.length == 0) {
        return false;
    }

    return false;
}

bool NetworkMonitor::IpHeaderMatches(const struct ip *ip_header) {
    const std::string ip_src = inet_ntoa(ip_header->ip_src);
    const std::string ip_dst = inet_ntoa(ip_header->ip_dst);

    return ip_src.rfind(_ip_filter) == 0 || ip_dst.rfind(_ip_filter) == 0;
}

const struct ip *NetworkMonitor::FindIpHeader(const void *raw_packet, size_t size)
{
    if (NetworkMonitor::IpHeaderMatches((const struct ip *)raw_packet)) {
        return (const struct ip *)raw_packet;
    }

    raw_packet = (const void *)(((const uint8_t *)raw_packet) + NetworkMonitor::ANY_HEADER_SIZE);
    if (NetworkMonitor::IpHeaderMatches((const struct ip *)raw_packet)) {
        return (const struct ip *)raw_packet;
    }

    return NULL;
}

void NetworkMonitor::ThreadFunction() {
    bool prev_server_alive = false;

    while (_running) {
        bool server_alive = HasServerConnection();
        if (prev_server_alive && !server_alive) {
            spdlog::info("Disconnected from {}", _ip_filter);
        }

        if (!prev_server_alive && server_alive) {
            spdlog::info("Connected to {}", _ip_filter);
        }

        prev_server_alive = server_alive;

        struct pcap_pkthdr header;
        const uint8_t *raw_packet = pcap_next(_pcap, &header);
        if (raw_packet == NULL) {
            continue;
        }

        spdlog::info("Captured packet of length {} from {}.x", header.caplen, _ip_filter);
        size_t raw_packet_size = header.caplen;

        if (raw_packet_size < NetworkMonitor::REQUIRED_HEADER_SIZE) {
            spdlog::info("Packet size {} is too small to parse (required size {})",
                raw_packet_size, NetworkMonitor::REQUIRED_HEADER_SIZE);
            continue;
        }

        const struct ip *ip_header = FindIpHeader(raw_packet, raw_packet_size);
        if (ip_header == NULL) {
            continue;
        }

        spdlog::info("DANK MEMES");

        // const struct tcp_hdr *tcp_header = FindTcpHeader(ip_header);
        // if (tcp_header == NULL) {
        //     continue;
        // }

        // const void *payload = FindTcpPayload(tcp_header);
        // size_t payload_size = FindTcpPayloadSize(tcp_header);
    }
}
