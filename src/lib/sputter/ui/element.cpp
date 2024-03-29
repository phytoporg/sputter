#include "element.h"
#include "events.h"

#include <cstring>

#include <sputter/system/system.h>
#include <sputter/log/log.h>

using namespace sputter::ui;
using namespace sputter::math;

Element::Element()
{}

Element::Element(Element* pParent)
    : m_pParent(pParent)
{
    memset(m_children, 0, sizeof(m_children));

    if (m_pParent)
    {
        if (!m_pParent->AddChild(this))
        {
            system::LogAndFail("Failed to add button to parent element.");
        }
    }
}

Element::~Element()
{
    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
    }
}

void Element::Draw() 
{
    if (!m_isVisible)
    {
        return;
    }

    DrawInternal();

    // Draw all children
    for (uint32_t i = 0; i < m_numChildren; ++i)
    {
        m_children[i]->Draw();
    }
}

void Element::Tick(float dt) 
{
    TickInternal(dt);

    // Handle events *after* ticking
    FlushEvents();

    // Tick all children
    for (int32_t i = 0; i < m_numChildren; ++i)
    {
        // Children can be removed during Tick calls
        Element* pChild = m_children[i];
        if (pChild)
        {
            pChild->Tick(dt);
        }
    }
}

void Element::QueueEvent(const Event& ev)
{
    m_eventQueue.push_back(ev);
}

void Element::SetPosition(const math::Vector2i& position)
{
    m_positionX = position.GetX();
    m_positionY = position.GetY();
}

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

void Element::SetDimensions(const math::Vector2i& dimensions)
{
    m_width = dimensions.GetX();
    m_height = dimensions.GetY();
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

Vector2i Element::GetDimensions() const { return Vector2i(m_width, m_height); }

uint32_t Element::GetWidth() const { return m_width; }

uint32_t Element::GetHeight() const { return m_height; }

Vector2i Element::GetAbsolutePosition() const
{
    return Vector2i(m_positionX, m_positionY) + (m_pParent ? m_pParent->GetAbsolutePosition() : Vector2i::Zero);
}

bool Element::AddChild(Element* pChildElement)
{
    if (m_numChildren >= kMaxChildren)
    {
        RELEASE_LOGLINE_WARNING(LOG_UI, "Cannot add child element, already at maximum number of children.");
        return false;
    }

    // Do not allow duplicate entries
    for (uint32_t i = 0; i < m_numChildren; ++i)
    {
        if (m_children[i] == pChildElement)
        {
            RELEASE_LOGLINE_WARNING(LOG_UI, "Attempting to add duplicate child element");
            return false;
        }
    }

    m_children[m_numChildren] = pChildElement;
    ++m_numChildren;

    // Signal the concrete implementation
    QueueEvent({EventCode::ChildAdded, pChildElement});
    
    return true;
}

bool Element::RemoveChild(Element* pChildElement)
{
    if (m_numChildren <= 0)
    {
        RELEASE_LOGLINE_WARNING(LOG_UI, "Attempting to remove child from element with no children");
        return false;
    }

    int elementIndex = -1;
    for (uint32_t i = 0; i < m_numChildren; ++i)
    {
        if (pChildElement == m_children[i])
        {
            elementIndex = static_cast<int>(i);
            break;
        }
    }

    if (elementIndex < 0)
    {
        RELEASE_LOGLINE_WARNING(LOG_UI, "Could not find matching child element");
        return false;
    }

    --m_numChildren;
    m_children[elementIndex] = m_children[m_numChildren];
    m_children[m_numChildren] = nullptr;

    // Signal the concrete implementation
    QueueEvent({EventCode::ChildRemoved, pChildElement});
    
    return true;
}

Element* Element::GetParent()
{
    return m_pParent;
}

const Element* Element::GetParent() const
{
    return m_pParent;
}

uint32_t Element::GetElementDepth() const
{
    uint32_t depth = 0;
    const Element* pParent = GetParent();
    while (pParent)
    {
        pParent = pParent->GetParent();
        ++depth;
    }

    return depth;
}

void Element::SetVisibility(bool isVisible)
{
    m_isVisible = isVisible;
}

float Element::GetRenderDepth() const
{
    return -1.0f * GetElementDepth();
}

void Element::FlushEvents()
{
    while (!m_eventQueue.empty())
    {
        const Event& event = m_eventQueue.front();
        HandleEvent(EventCodeToParameter(event.Code), event.pData);
        m_eventQueue.pop_front();
    }
}

void Element::SignalRootElement(const Event& event)
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
