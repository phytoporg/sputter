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

    uint8_t id;
    pServer->GetClientId(clientHandle, id);

    std::string name;
    pServer->GetClientName(clientHandle, name);

    RELEASE_LOGLINE_INFO(
        LOG_NET,
        "NEW CONNECTION! Handle = %u, Name = %s, Address = %s:%d, Id = %hhu",
        clientHandle,
        name.c_str(),
        address.c_str(),
        port,
        id);
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
