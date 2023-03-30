#include <stdlib.h>
#include <signal.h>

#include <thread>
#include <atomic>

#include <spdlog/spdlog.h>

#include "SoftwareBusSubsystem.h"
#include "NetworkMonitor.h"

using namespace std::literals::chrono_literals;
std::atomic<bool> g_killswitch;

void SignalHandler(int signum) {
    if (!g_killswitch) {
        g_killswitch = true;
    } else {
        spdlog::info("Ctrl+C double input - exiting immediately.");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    signal(SIGINT, SignalHandler);

    SoftwareBusSubsystem software_bus;
    NetworkMonitor network_monitor(NetworkMonitor::NA_DATA_CENTER_ADDRESS, software_bus);

    software_bus.Init();
    network_monitor.Init();

    spdlog::info("Press Ctrl+C to exit.");

    while (!g_killswitch) {
        std::this_thread::sleep_for(10ms);
    }

    spdlog::info("Ctrl+C pressed. Exiting.");

    network_monitor.Teardown();
    software_bus.Teardown();
}
