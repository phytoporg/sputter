#include "screen.h"

#include <sputter/render/window.h>
#include <sputter/system/system.h>
#include <sputter/log/log.h>

#include <GLFW/glfw3.h>

using namespace sputter;
using namespace sputter::ui;

Screen::Screen(render::Window* pWindow)
    : m_pWindow(pWindow),
      m_pKeyMapping(nullptr)
{
    m_pKeyMapping = new Key[GLFW_KEY_LAST];
    m_pKeyMapping[GLFW_KEY_UP]    = Key::Up;
    m_pKeyMapping[GLFW_KEY_DOWN]  = Key::Down;
    m_pKeyMapping[GLFW_KEY_LEFT]  = Key::Left;
    m_pKeyMapping[GLFW_KEY_RIGHT] = Key::Right;
    m_pKeyMapping[GLFW_KEY_W]     = Key::Up;
    m_pKeyMapping[GLFW_KEY_S]     = Key::Down;
    m_pKeyMapping[GLFW_KEY_A]     = Key::Left;
    m_pKeyMapping[GLFW_KEY_D]     = Key::Right;
    m_pKeyMapping[GLFW_KEY_ENTER] = Key::Activate;
    m_pKeyMapping[GLFW_KEY_SPACE] = Key::Activate;
}

Screen::~Screen()
{
    m_pWindow->SetKeyCallback(0);

    delete[] m_pKeyMapping;
    m_pKeyMapping = nullptr;

    // TODO: Destroy element hierarchy?
}

void Screen::Initialize()
{
    // Need this layer of indirection because the glfw user pointer is 
    // already taken by our render::Window pointer. Handlers could be inlined 
    // here in lambdas, but I'd prefer to keep the constructor succinct and clear.
    m_pWindow->SetKeyCallback([this](render::Window* pWindow, int key, int action) {
        HandleKeyEvent(key, action);
    });
    SetVisibility(true);
    FlushEvents();
}

void Screen::Uninitialize()
{
    m_pWindow->SetKeyCallback(0);
    SetVisibility(false);
    FlushEvents();
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
    const EventCode event = ParameterToEventCode(eventCode);
    if (event == EventCode::ChildAdded)
    {
        if (!m_pElementInFocus)
        {
            m_pElementInFocus = static_cast<Element*>(pEventData);
            m_pElementInFocus->QueueEvent({EventCode::FocusBegin});
        }
    }
    else if (event == EventCode::ChildRemoved)
    {
        if (m_pElementInFocus == static_cast<Element*>(pEventData))
        {
            m_pElementInFocus->QueueEvent({EventCode::FocusEnd});
        }

        m_pElementInFocus = nullptr;

        // TODO: Who gets focus now, though?
    }
    else if (event == EventCode::FocusBegin)
    {
        Element* pNewFocusElement = static_cast<Element*>(pEventData);
        if (pNewFocusElement != m_pElementInFocus)
        {
            m_pElementInFocus->QueueEvent({EventCode::FocusEnd});
            m_pElementInFocus = pNewFocusElement;

            // No payload required for non-root elements.
            m_pElementInFocus->QueueEvent({EventCode::FocusBegin});
        }
    }
}

void Screen::HandleKeyEvent(int key, int action)
{
    if (!m_pElementInFocus)
    {
        // Nothing to do if no child elements are in focus.
        return;
    }

    EventCode keyEvent = EventCode::Invalid;

    Key keyPressed = m_pKeyMapping[key];
    if (action == GLFW_PRESS)
    {
        keyEvent = EventCode::KeyDown;
    }
    else if (action == GLFW_REPEAT)
    {
        keyEvent = EventCode::KeyHeld;
    }
    else if (action == GLFW_RELEASE)
    {
        keyEvent = EventCode::KeyUp;
    }

    if (keyEvent == EventCode::Invalid)
    {
        RELEASE_LOG_ERROR(LOG_FONT, "Unexpected key action: 0x%x", keyEvent);
        return;
    }

    if (keyPressed == Key::Invalid)
    {
        // Unhandled key event
        return;
    }
    
    m_pElementInFocus->HandleEvent(EventCodeToParameter(keyEvent), &keyPressed);
}
