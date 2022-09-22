#include "button.h"
#include "element.h"
#include "events.h"
#include "navigation.h"

#include <sputter/render/drawshapes.h>
#include <sputter/system/system.h>

#include <cstring>

using namespace sputter::ui;

Button::Button(Element* pParent, Theme* pTheme)
    : Element(pParent),
      m_pTheme(pTheme)
{
    memset(m_pNavLinks, 0, sizeof(m_pNavLinks));
    m_borderColor = m_pTheme->UnfocusedBorderColor;
}

void Button::HandleEvent(uint8_t eventCode, void* pEventData)
{
    EventCode event = ParameterToEventCode(eventCode);
    if (event == EventCode::FocusBegin)
    {
        m_borderColor = m_pTheme->FocusedBorderColor;
    }
    else if (event == EventCode::FocusEnd)
    {
        m_borderColor = m_pTheme->UnfocusedBorderColor;
    }
    else if (event == EventCode::KeyUp)
    {
        const Key KeyReleased = *static_cast<Key*>(pEventData);
        if (KeyReleased <= Key::Invalid || KeyReleased >= Key::KeyMax)
        {
            LOG(ERROR) << "Unhandled key release value made it to the button handler";
            return;
        }

        static const NavigationDirections KeyToDirectionMap[static_cast<uint8_t>(Key::KeyMax)] = {
            NavigationDirections::Invalid,
            NavigationDirections::Up,
            NavigationDirections::Down
        };

        // If we get this, we should be the element in focus
        const uint8_t KeyReleasedIndex = static_cast<uint8_t>(KeyReleased);
        const uint8_t NavLinkIndex = static_cast<uint8_t>(KeyToDirectionMap[KeyReleasedIndex]) - 1;
        Button* pNavButton = m_pNavLinks[NavLinkIndex];
        if (pNavButton)
        {
            Event event{ EventCode::FocusBegin, pNavButton };
            SignalRootElement(event);
        }
    }
}

void Button::SetBorderSize(uint8_t borderSize)
{
    m_borderSize = borderSize;
}

void Button::SetNavigationLink(Button* pOtherButton, NavigationDirections direction)
{
    if (direction <= NavigationDirections::Invalid || direction >= NavigationDirections::DirectionMax)
    {
        system::LogAndFail("Invalid navigation link direction");
    }

    if (pOtherButton != this)
    {
        const uint8_t NavLinkIndex = static_cast<uint8_t>(direction) - 1;
        m_pNavLinks[NavLinkIndex] = pOtherButton;
    }
}

void Button::DrawInternal()
{
    const auto AbsolutePosition = GetAbsolutePosition();
    sputter::render::shapes::DrawRect(
        AbsolutePosition.GetX(), AbsolutePosition.GetY(),
        GetWidth(), GetHeight(),
        m_borderSize, m_borderColor );
}

void Button::TickInternal(float dt)
{
    // TODO
}

void Button::SignalRootElement(const Event& event)
{
    Element* pRoot = this;
    while(true)
    {
        Element* pParent = pRoot->GetParent();
        if (!pParent)
        {
            break;
        }
        
        pRoot = pRoot->GetParent();
    }

    pRoot->QueueEvent(event);
}