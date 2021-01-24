// Main window class, assuming one singular window to host the rendering
// context.

#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace sputter { namespace render {
    const uint32_t kDefaultWindowWidth  = 1024;
    const uint32_t kDefaultWindowHeight = 768;

    class Window
    {
        public:
            Window(const std::string& windowTitle);
            Window(const std::string& windowTitle, uint32_t w, uint32_t h);
            ~Window();

            void EnableInputs();
            bool GetKeyState(uint32_t keyCode);

            void Clear();
            void Tick();

            uint32_t GetWidth() const;
            uint32_t GetHeight() const;

            bool ShouldClose();

        private:
            static bool m_isInitialized;
            GLFWwindow* m_window;

            uint32_t m_width;
            uint32_t m_height;
    };
}}

