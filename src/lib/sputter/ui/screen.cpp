#include "screen.h"

using namespace sputter::ui;

Screen::Screen(render::Window* pWindow)
    : m_pWindow(pWindow)
{}

Screen::~Screen()
{
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
