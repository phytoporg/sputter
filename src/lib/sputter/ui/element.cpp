#include "element.h"

using namespace sputter::ui;

Element::Element()
{}

Element::Element(Element* pParent)
    : m_pParent(pParent)
{}

Element::~Element()
{}

void Element::SetPosition(uint32_t x, uint32_t y)
{
    m_positionX = x;
    m_positionY = y;
}

void Element::SetPositionX(uint32_t x)
{
    m_positionX = x;
}

void Element::SetPositionY(uint32_t y)
{
    m_positionY = y;
}

void Element::SetDimensions(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
}

void Element::SetWidth(uint32_t width)
{
    m_width = width;
}

void Element::SetHeight(uint32_t height)
{
    m_height = height;
}