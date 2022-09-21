#pragma once

#include "element.h"

// Forward declarations
namespace sputter
{
    namespace render
    {
        class Window;
    }
}

namespace sputter { namespace ui {
    // Screen will typically be the root element in an element hierarchy.
    class Screen : public Element
    {
    public:
        Screen(render::Window* pWindow);
        virtual ~Screen();

        virtual void HandleEvent(uint8_t eventCode, void* pEventData) override;

    protected:
        virtual void DrawInternal() override;
        virtual void TickInternal(float dt) override;

    private:
        render::Window* m_pWindow = nullptr;
    };
}}
