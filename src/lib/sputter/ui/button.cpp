#include "button.h"
#include "element.h"

using namespace sputter::ui;

Button::Button(Element* pParent)
    : Element(pParent)
{}

void Button::Tick(float dt)
{
    // TODO
}

void Button::Draw()
{
    // TODO
}

void Button::HandleEvent(uint32_t eventCode, void* pEventData)
{
    // TODO
}

void Button::SetBorderWidth(uint8_t borderWidth)
{
    // TODO
}

void Button::SetBorderColor(uint8_t r, uint8_t g, uint8_t b)
{
    // TODO
}

void Button::SetBorderColor(const render::Color& color)
{
    // TODO
}