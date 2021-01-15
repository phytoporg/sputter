#include <sputter/render/window.h>
#include <sputter/system/system.h>

using namespace sputter;

int main(int argc, char** argv)
{
    system::InitializeLogging(argv[0]);
    render::Window window("Test window");

    window.EnableInputs();
    while (!window.ShouldClose() && !window.GetKeyState(GLFW_KEY_ESCAPE))
    {
        window.Clear();
        window.Tick();
    }

    return 0;
}
