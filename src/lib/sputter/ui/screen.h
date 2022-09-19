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

        virtual void Tick(float dt) override;
        virtual void HandleEvent(uint32_t eventCode, void* pEventData) override;

    protected:
        virtual void DrawInternal() override;

    private:
        render::Window* m_pWindow = nullptr;
    };
}}