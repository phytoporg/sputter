// TODO: Find a better place for this. Shouldn't have to define this in the specific game project.
#define NOMINMAX

#include "paddlearena.h"
#include "gamemode.h"

#include <sputter/core/cliargumentparser.h>

#include <sputter/render/spritesubsystem.h>
#include <sputter/render/window.h>

#include <sputter/physics/rigidbody2d.h>

#include <sputter/system/system.h>
#include <sputter/system/time.h>

#include "gameconstants.h"

#include <iostream>

using namespace sputter;

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);
    log::EnableZone(log::LogZone::Game);
    log::DisableZone(log::LogZone::Font);
    log::DisableZone(log::LogZone::Mesh);
    log::EnableZone(log::LogZone::Net);
    log::EnableZone(log::LogZone::Serializer);

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <asset_path> "
                  << "--log-path <log_path> "
                  << "--log-verbosity <Error|Warning|Info|Verbose|VeryVerbose> "
                  << "--p2p-server "
                  << "--p2p-client <server_ip:server_port> "
                  << std::endl;
        return -1;
    }

    GameMode gameMode = GameMode::Local; // Game plays local by default
    core::CommandLineArgumentParser parser(argc, argv);
    if (const core::CommandLineArgument* pArgument = parser.FindArgument("log-path"))
    {
        log::SetLogFile(pArgument->AsString().c_str());
    }

    if (const core::CommandLineArgument* pArgument = parser.FindArgument("log-verbosity"))
    {
        log::SetLogVerbosityFromString(pArgument->AsString().c_str());
    }

    if (const core::CommandLineArgument* pArgument = parser.FindArgument("p2p-server"))
    {
        gameMode = GameMode::Server;
        RELEASE_LOGLINE_INFO(LOG_DEFAULT, "PaddleArena starting in server mode");
    }

    const int32_t kDefaultServerPort = 7001; // TODO: Put in some kinda server class
    std::string remoteServerAddress;
    int32_t remoteServerPort = kDefaultServerPort;
    if (const core::CommandLineArgument* pArgument = parser.FindArgument("p2p-client"))
    {
        if (gameMode == GameMode::Server)
        {
            RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "p2p-client and p2p-server are mutually exclusive");
            return -1;
        }
        gameMode = GameMode::Client;

        const std::string& ArgValue = pArgument->AsString();
        const size_t ColonPosition = ArgValue.find(':');
        if (ColonPosition != std::string::npos)
        {
            if (ColonPosition == 0 || ColonPosition == (ArgValue.size() - 1))
            {
                RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Malformed server address: %s", ArgValue.c_str());
                return -1;
            }

            const std::string PortString = ArgValue.substr(ColonPosition + 1);
            remoteServerPort = std::atoi(PortString.c_str());
            remoteServerAddress = ArgValue.substr(0, ColonPosition);
        }
        else
        {
            remoteServerAddress = ArgValue;
        }

        RELEASE_LOGLINE_INFO(LOG_DEFAULT, "PaddleArena starting in client mode");
        RELEASE_LOGLINE_INFO(
            LOG_DEFAULT,
            "Remote server: %s:%d",
            remoteServerAddress.c_str(), remoteServerPort);
    }

    if (gameMode == GameMode::Local)
    {
        RELEASE_LOGLINE_INFO(
            LOG_DEFAULT,
            "PaddleArena starting in local mode",
            remoteServerAddress.c_str(), remoteServerPort);
    }

    render::Window window("PADDLEARENA", gameconstants::OrthoWidth, gameconstants::OrthoHeight);
    PaddleArena game(&window, argv[1], gameMode, remoteServerAddress, remoteServerPort);

    game.StartGame();

    window.EnableInputs();

    const math::FixedPoint DesiredFps  = math::FPSixty;
    const math::FixedPoint FrameStepMs = math::FixedPoint(1000) / DesiredFps;
    const math::FixedPoint DeltaTime = math::FPOne / DesiredFps;
    uint32_t nextTick = 
        system::GetTickMilliseconds() + static_cast<uint32_t>(FrameStepMs);
    while (!window.ShouldClose() && !window.GetKeyState(GLFW_KEY_ESCAPE))
    {
        window.Clear();

        game.Tick(DeltaTime);
        game.Draw();

        window.Tick();

        const uint32_t TimeMs = system::GetTickMilliseconds();
        if (TimeMs < nextTick)
        {
            system::SleepMs(nextTick - TimeMs);
        }
        nextTick = TimeMs + static_cast<uint32_t>(FrameStepMs);
    }

    return 0;
}
