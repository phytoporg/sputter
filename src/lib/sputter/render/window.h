// Main window class, assuming one singular window to host the rendering
// context.

#pragma once

#include "render.h"
#include <string>
#include <functional>

struct GLFWwindow;

namespace sputter { namespace render {
    const uint32_t kDefaultWindowWidth  = 1024;
    const uint32_t kDefaultWindowHeight = 768;

    class Window
    {
        public:
            Window(const std::string& windowTitle);
            Window(const std::string& windowTitle, uint32_t w, uint32_t h);
            ~Window();

            using WindowResizeCallback = std::function<void(Window*, uint32_t, uint32_t)>;
            void SetWindowResizeCallback(const WindowResizeCallback onResize);

            using WindowKeyCallback = std::function<void(Window*, int, int)>;
            void SetWindowKeyCallback(const WindowKeyCallback onKey);

            // Pass in -1 for any unconstrained parameters
            void SetDimensionConstraints(
                int minWidth, int minHeight,
                int maxWidth, int maxHeight);

            void EnableInputs();
            bool GetKeyState(uint32_t keyCode);

            void Clear();
            void Tick();

            uint32_t GetWidth() const;
            uint32_t GetHeight() const;

            bool ShouldClose();

        private:
            static void WindowResizeThunk(GLFWwindow* pWindow, int width, int height);
            WindowResizeCallback m_fnWindowResizeCallback;

            static void WindowKeyThunk(GLFWwindow* pWindow,
                int key, int scancode, int action, int mods);
            WindowKeyCallback m_fnWindowKeyCallback;

            // Tracks whether glfw is initialized. Not sure if this belongs here?
            static bool m_isInitialized;
            GLFWwindow* m_pWindow;

            uint32_t m_width;
            uint32_t m_height;
    };
}}

