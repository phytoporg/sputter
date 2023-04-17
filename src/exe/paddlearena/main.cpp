// TODO: Find a better place for this. Shouldn't have to define this in the specific game project.
#define NOMINMAX

#include "paddlearena.h"

#include <sputter/core/cliargumentparser.h>

#include <sputter/assets/imagedata.h>
#include <sputter/assets/assetstorage.h>

#include <sputter/render/spritesubsystem.h>
#include <sputter/render/sprite.h>
#include <sputter/render/texturestorage.h>
#include <sputter/render/window.h>

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/physics/rigidbody2d.h>

#include <sputter/memory/reservedregion.h>

#include <sputter/system/system.h>
#include <sputter/system/time.h>

#include "gameconstants.h"

#include <iostream>

using namespace sputter;

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);
    // REMOVEME
    log::SetLogFile("/tmp/logout");
    // REMOVEME

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " " << "<asset_path>" << std::endl;
        return -1;
    }

    core::CommandLineArgumentParser parser(argc, argv);
    if (const core::CommandLineArgument* pArgument = parser.FindArgument("log-path"))
    {
        log::SetLogFile(pArgument->AsString().c_str());
    }

    render::Window window("PADDLEARENA", gameconstants::OrthoWidth, gameconstants::OrthoHeight);
    PaddleArena game(&window, argv[1]);

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
