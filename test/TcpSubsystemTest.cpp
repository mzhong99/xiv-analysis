#include <doctest.h>
#include <spdlog/spdlog.h>

#include "SoftwareBusSubsystem.h"
#include "TcpServerSubsystem.h"
#include "TcpClientSubsystem.h"

TEST_CASE("TcpSubsystem_InitServer") {
    TcpServerSubsystem server;
    server.Init(42069);
    server.Teardown();
}

TEST_CASE("TcpSubsystem_InitSingleClientServer") {
    TcpServerSubsystem server;

    TcpClientSubsystem real_client;
    TcpClientSubsystem fork_client;

    SoftwareBusSubsystem software_bus;

    server.Init(42069);
    real_client.Init();
    fork_client.InitFromServer();

    fork_client.Teardown();
    real_client.Teardown();
    server.Teardown()
}
