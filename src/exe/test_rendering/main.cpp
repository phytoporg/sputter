#include <sputter/render/window.h>

using namespace sputter;

int main()
{
    render::Window window("Test window");

    window.EnableInputs();
    while (!window.ShouldClose() && !window.GetKeyState(GLFW_KEY_ESCAPE))
    {
        window.Clear();
        window.Tick();
    }

    return 0;
}
