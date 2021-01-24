#include "window.h"
#include <GL/gl.h>
#include <glog/logging.h>
#include <sputter/system/system.h>

#include <stdexcept>

namespace sputter { namespace render {
    bool Window::m_isInitialized = false;
    
    Window::Window(const std::string& windowTitle)
        : Window(windowTitle, kDefaultWindowWidth, kDefaultWindowHeight)
    {}

    Window::Window(const std::string& windowTitle, uint32_t w, uint32_t h)
        : m_width(w), m_height(h)
    {
        if (glfwInit())
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

        m_window = glfwCreateWindow(w, h, windowTitle.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            system::LogAndFail("Failed to create main window.");
        }

        glfwMakeContextCurrent(m_window);

        if (glewInit())
        {
            system::LogAndFail("glewInit() failed.");
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }

    Window::~Window()
    {
        glfwTerminate();
    }

    void Window::EnableInputs()
    {
        glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    }

    bool Window::GetKeyState(uint32_t keyCode)
    {
        return glfwGetKey(m_window, keyCode) == GLFW_PRESS;
    }

    void Window::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Window::Tick()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
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
        return static_cast<bool>(glfwWindowShouldClose(m_window));
    }
}}

