#include "element.h"

using namespace sputter::ui;

Element::Element()
{}

Element::Element(Element* pParent)
    : m_pParent(pParent)
{}

void Element::SetPosition(uint32_t x, uint32_t y)
{
    m_positionX = x;
    m_positionY = y;
}

void Element::SetDimensions(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
}