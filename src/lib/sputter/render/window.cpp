#include "window.h"
#include "render.h"
#include <sputter/system/system.h>

#include <stdexcept>

using namespace sputter::render;

bool Window::m_isInitialized = false;

Window::Window(const std::string& windowTitle)
    : Window(windowTitle, kDefaultWindowWidth, kDefaultWindowHeight)
{}

Window::Window(const std::string& windowTitle, uint32_t w, uint32_t h)
    : m_width(w), m_height(h)
{
    if (m_isInitialized || glfwInit())
    {
        m_isInitialized = true;
    }
    else
    {
        system::LogAndFail("glfwInit() failed");
    }

    // No antialiasing
    glfwWindowHint(GLFW_SAMPLES, 1);

    // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

    // We don't want the old OpenGL 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    m_pWindow = glfwCreateWindow(w, h, windowTitle.c_str(), nullptr, nullptr);
    if (!m_pWindow)
    {
        system::LogAndFail("Failed to create main window.");
    }

    glfwMakeContextCurrent(m_pWindow);

    // Hide the mouse cursor
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (glewInit())
    {
        system::LogAndFail("glewInit() failed.");
    }

    // Set up thunks
    glfwSetWindowUserPointer(m_pWindow, this);

    // Context at https://www.glfw.org/docs/3.3/window_guide.html:
    // 
    // The window size callback produces dimensions in screen coordinates,
    // but we're interested in pixel sizes, so use frame buffer size change
    // events instead.
    glfwSetFramebufferSizeCallback(m_pWindow, WindowResizeThunk);
    glfwSetKeyCallback(m_pWindow, WindowKeyThunk);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

Window::~Window()
{
    glfwTerminate();
}

void Window::SetWindowResizeCallback(WindowResizeCallback onResize)
{
    m_fnWindowResizeCallback = onResize;
}

void Window::SetKeyCallback(WindowKeyCallback onKey)
{
    m_fnWindowKeyCallback = onKey;
}

void Window::SetDimensionConstraints(int minWidth, int minHeight, int maxWidth, int maxHeight)
{
    minWidth = minWidth < 0 ? GLFW_DONT_CARE : minWidth;
    minHeight = minHeight < 0 ? GLFW_DONT_CARE : minHeight;
    maxWidth = maxWidth < 0 ? GLFW_DONT_CARE : maxWidth;
    maxHeight = maxHeight < 0 ? GLFW_DONT_CARE : maxHeight;

    glfwSetWindowSizeLimits(m_pWindow, minWidth, minHeight, maxWidth, maxHeight);
}

void Window::EnableInputs()
{
    glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
}

bool Window::GetKeyState(uint32_t keyCode)
{
    const int keyState = glfwGetKey(m_pWindow, keyCode);
    return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
}

void Window::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::Tick()
{
    glfwPollEvents();
    glfwSwapBuffers(m_pWindow);
}

uint32_t Window::GetWidth() const
{
    return m_width;
}

uint32_t Window::GetHeight() const
{
    return m_height;
}

bool Window::ShouldClose()
{
    return static_cast<bool>(glfwWindowShouldClose(m_pWindow));
}

void Window::WindowResizeThunk(GLFWwindow* pWindow, int width, int height)
{
    if (!pWindow)
    {
        system::LogAndFail("Null window pointer in window resize thunk!");
    }

    auto pUserWindow = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
    if (pUserWindow)
    {
        pUserWindow->m_width = static_cast<uint32_t>(width);
        pUserWindow->m_height = static_cast<uint32_t>(height);

        if (pUserWindow->m_fnWindowResizeCallback)
        {
            pUserWindow->m_fnWindowResizeCallback(
                pUserWindow, pUserWindow->m_width, pUserWindow->m_height);
        }
    }
}

void Window::WindowKeyThunk(GLFWwindow* pWindow,
    int key, int scancode, int action, int mods)
{
    if (!pWindow)
    {
        system::LogAndFail("Null window pointer in window key thunk!");
    }

    auto pUserWindow = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
    if (pUserWindow)
    {
        if (pUserWindow->m_fnWindowKeyCallback)
        {
            pUserWindow->m_fnWindowKeyCallback(pUserWindow, key, action);
        }
    }
}
