#include "button.h"
#include "element.h"
#include "events.h"
#include "navigation.h"

#include <sputter/render/drawshapes.h>
#include <sputter/system/system.h>
#include <sputter/log/log.h>

#include <cstring>

using namespace sputter::ui;

// Small displacements for button down state
const int32_t kButtonDownDisplacementX = -1;
const int32_t kButtonDownDisplacementY = -1;
const sputter::math::Vector2i kButtonDownDisplacement(kButtonDownDisplacementX, kButtonDownDisplacementY);

Button::Button(Element* pParent, Theme* pTheme, const char* pText)
    : Element(pParent),
      m_pTheme(pTheme)
{
    memset(m_pNavLinks, 0, sizeof(m_pNavLinks));
    m_borderColor = m_pTheme->UnfocusedBorderColor;
    m_borderSize  = m_pTheme->ButtonBorderSize;

    memset(m_text, 0, sizeof(m_text));
    if (pText)
    {
        strncpy(&m_text[0], pText, kMaxTextLength);
    }
}

void Button::HandleEvent(uint8_t eventCodeParameter, void* pEventData)
{
    EventCode eventCode = ParameterToEventCode(eventCodeParameter);
    if (eventCode == EventCode::FocusBegin)
    {
        m_borderColor = m_pTheme->FocusedBorderColor;
    }
    else if (eventCode == EventCode::FocusEnd)
    {
        m_borderColor = m_pTheme->UnfocusedBorderColor;
    }
    else if (eventCode == EventCode::KeyDown)
    {
        // Not that receiving key events implies that this is the element in focus.
        const Key KeyPressed = *static_cast<Key*>(pEventData);
        if (KeyPressed <= Key::Invalid || KeyPressed >= Key::KeyMax)
        {
            RELEASE_LOG_ERROR_(LOG_UI, "Unhandled key press value made it to the button handler.");
            return;
        }

        if (IsDirectionKey(KeyPressed))
        {
            static const NavigationDirections KeyToDirectionMap[static_cast<uint8_t>(Key::KeyMax)] = {
                NavigationDirections::Invalid,
                NavigationDirections::Up,
                NavigationDirections::Down,
                NavigationDirections::Left,
                NavigationDirections::Right,
            };

            const uint8_t KeyPressedIndex = static_cast<uint8_t>(KeyPressed);
            const uint8_t NavLinkIndex = static_cast<uint8_t>(KeyToDirectionMap[KeyPressedIndex]) - 1;
            Button* pNavButton = m_pNavLinks[NavLinkIndex];
            if (pNavButton)
            {
                SignalRootElement(Event{ EventCode::FocusBegin, pNavButton });
            }
        }
        else if (KeyPressed == Key::Activate && m_buttonState == ButtonState::Idle)
        {
            QueueEvent(Event{ EventCode::Activate });
        }
    }
    else if (eventCode == EventCode::KeyUp)
    {
        if (m_buttonState == ButtonState::Down)
        {
            QueueEvent(Event{ EventCode::Deactivate });
        }
    }
    else if (eventCode == EventCode::Activate)
    {
        if (m_buttonState != ButtonState::Idle)
        {
            RELEASE_LOG_ERROR_(LOG_UI, "Button received activation event while not idle");
            return;
        }

        m_buttonState = ButtonState::Down;

        // TODO: Should indicate disabled state more clearly and without requiring user
        // interaction! Need some form of feedback right now, though.
        if (m_buttonDisabled)
        {
            m_borderColor = m_pTheme->ButtonDownAndDisabledBorderColor;
        }
        else
        {
            // For just a little extra differentiation
            m_borderColor = m_pTheme->UnfocusedBorderColor;
        }
    }
    else if (eventCode == EventCode::Deactivate)
    {
        if (m_buttonState != ButtonState::Down)
        {
            RELEASE_LOG_ERROR_(LOG_UI, "Button received deactivation event while not held down");
            return;
        }

        m_buttonState = ButtonState::Idle;

        // For just a little extra differentiation
        m_borderColor = m_pTheme->FocusedBorderColor;

        // This resolves to a button press event !
        if (!m_buttonDisabled && m_fnButtonPressed)
        {
            m_fnButtonPressed();
        }
    }
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

void Button::SetFontRenderer(render::VolumetricTextRenderer* pTextRenderer)
{
    m_pTextRenderer = pTextRenderer;
}

void Button::SetButtonPressedCallback(const ButtonPressedCallback onButtonPressed)
{
    m_fnButtonPressed = onButtonPressed;
}

void Button::SetButtonIsDisabled(bool isDisabled)
{
    m_buttonDisabled = isDisabled;
}

void Button::DrawInternal()
{
    using namespace sputter::render;

    const auto AbsolutePosition = GetAbsolutePosition();
    const auto PositionToRender = AbsolutePosition + 
        (m_buttonState == ButtonState::Down ?
            kButtonDownDisplacement : math::Vector2i::Zero);
    {
        const float PreviousLineRenderDepth = shapes::GetLineRendererDepth();
        shapes::SetLineRendererDepth(GetRenderDepth());

        sputter::render::shapes::DrawRect(
            PositionToRender.GetX(), PositionToRender.GetY(),
            GetWidth(), GetHeight(),
            m_borderSize, m_borderColor);

        shapes::SetLineRendererDepth(PreviousLineRenderDepth);
    }

    if (m_text[0] && m_pTextRenderer)
    {
        const float PreviousTextDepth = m_pTextRenderer->GetDepth();
        m_pTextRenderer->SetDepth(-1.0f * GetElementDepth());

        const uint32_t kButtonTextSize = 1;
        m_pTextRenderer->DrawTextCentered(
            PositionToRender.GetX(),
            PositionToRender.GetX() + GetWidth(),
            PositionToRender.GetY() + (GetHeight() / 2),
            kButtonTextSize, m_text);

        m_pTextRenderer->SetDepth(PreviousTextDepth);
    }
}

void Button::TickInternal(float dt)
{
    // TODO
}