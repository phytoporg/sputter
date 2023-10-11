#include "server.h"

#include <sputter/log/log.h>
#include <sputter/system/system.h>

using namespace sputter;

void OnClientConnected(Server::ClientHandle clientHandle)
{
    RELEASE_LOGLINE_INFO(LOG_NET, "NEW CONNECTION! Handle = %u", clientHandle);
}

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);
    log::EnableZone(log::LogZone::Net);
    log::SetLogVerbosity(log::LogVerbosity::Info);

    Server server(OnClientConnected);
    if (!server.Listen())
    {
        RELEASE_LOG_ERROR(
            LOG_NET,
            "Server failed to listen on port %d!",
            server.GetPort());
        return -1;
    }

    bool shouldLoop = true;
    while (shouldLoop)
    {
        server.Tick();
        shouldLoop = !server.ShouldTerminate();
    }

    return 0;
}
