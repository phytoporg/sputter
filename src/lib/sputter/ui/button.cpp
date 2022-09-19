#include "button.h"
#include "element.h"

#include <sputter/render/drawshapes.h>

using namespace sputter::ui;

Button::Button(Element* pParent)
    : Element(pParent)
{}

void Button::HandleEvent(uint32_t eventCode, void* pEventData)
{
    // TODO
}

void Button::SetBorderSize(uint8_t borderSize)
{
    m_borderSize = borderSize;
}

void Button::SetBorderColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_borderColor = render::Color{r, g, b};
}

void Button::SetBorderColor(const render::Color& color)
{
    m_borderColor = color;
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

