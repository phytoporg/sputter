#include "screen.h"

#include <sputter/render/window.h>
#include <sputter/system/system.h>

 #include <GLFW/glfw3.h>

using namespace sputter;
using namespace sputter::ui;

Screen::Screen(render::Window* pWindow)
    : m_pWindow(pWindow),
      m_pKeyMapping(nullptr)
{
    m_pKeyMapping = new Key[GLFW_KEY_LAST];
    m_pKeyMapping[GLFW_KEY_UP]   = Key::Up;
    m_pKeyMapping[GLFW_KEY_DOWN] = Key::Down;

    // Need this layer of indirection because the glfw user pointer is already taken by our
    // render::Window pointer. Handlers could be inlined here in lambdas, but I'd prefer to keep
    // the constructor succinct and clear.
    m_pWindow->SetKeyCallback([this](render::Window* pWindow, int key, int action) {
        HandleKeyEvent(key, action);
    });
}

Screen::~Screen()
{
    delete[] m_pKeyMapping;
    m_pKeyMapping = nullptr;

    // TODO: Destroy element hierarchy?
}

void Screen::TickInternal(float dt) 
{
    // TODO
}

void Screen::DrawInternal() 
{
    // Nothing to draw for the screen
}

void Screen::HandleEvent(uint8_t eventCode, void* pEventData)
{
    // TODO
}

void Screen::HandleKeyEvent(int key, int action)
{
    if (!m_pElementInFocus)
    {
        // Nothing to do if no child elements are in focus.
        return;
    }

    Event keyEvent = Event::Invalid;

    Key keyPressed = m_pKeyMapping[key];
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        keyEvent = Event::KeyDown;
    }
    else if (action == GLFW_RELEASE)
    {
        keyEvent = Event::KeyUp;
    }

    if (keyEvent == Event::Invalid)
    {
        LOG(ERROR) << "Unexpected key action";
        return;
    }
    
    m_pElementInFocus->HandleEvent(static_cast<uint8_t>(keyEvent), static_cast<void*>(&keyPressed));
}