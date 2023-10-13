#include "server.h"

#include <sputter/log/log.h>
#include <sputter/system/system.h>

using namespace sputter;

void OnClientConnected(Server* pServer, Server::ClientHandle clientHandle)
{
    std::string address;
    pServer->GetClientAddress(clientHandle, address);

    int port;
    pServer->GetClientPort(clientHandle, port);

    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "NEW CONNECTION! Handle = %u, Address = %s:%d",
        clientHandle,
        address.c_str(),
        port);
}

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);
    log::EnableZone(log::LogZone::Net);
    log::SetLogVerbosity(log::LogVerbosity::Verbose);

    Server server(OnClientConnected);
    if (!server.Listen())
    {
        RELEASE_LOG_ERROR(
            LOG_NET,
            "Server failed to listen on port %d!",
            server.GetPort());
        return -1;
    }

    RELEASE_LOGLINE_INFO(LOG_NET, "Server is listening on port %d", server.GetPort());

    bool shouldLoop = true;
    while (shouldLoop)
    {
        server.Tick();
        shouldLoop = !server.ShouldTerminate();
    }

    return 0;
}
