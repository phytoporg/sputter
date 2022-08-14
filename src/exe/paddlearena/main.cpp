// TODO: Find a better place for this. Shouldn't have to define this in the specific game project.
#define NOMINMAX

#include "paddlearena.h"

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

#include <iostream>

using namespace sputter;

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " " << "<asset_path>" << std::endl;
        return -1;
    }

    memory::ReservedRegion stateRegion(0x100000);
    memory::FixedMemoryAllocator allocator(
        "GameState",
        stateRegion.GetRegionBase(),
        stateRegion.GetRegionSize());

    render::Window window("Test window");
    PaddleArena game(&window, argv[1], allocator);

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
