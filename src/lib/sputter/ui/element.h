#pragma once

#include <sputter/math/vector2i.h>

#include <deque>

#include <cstdint>

namespace sputter 
{
    namespace ui
    {
        struct Event;
        class Element
        {
        public:
            // For root elements with no parent
            Element();
            Element(Element* pParent);

            virtual ~Element();

            void Draw();
            void Tick(float dt);
            void QueueEvent(const Event& event);

            virtual void HandleEvent(uint8_t eventCode, void* pEventData = nullptr) = 0;

            // These positions are relative to the parent element
            void SetPosition(const math::Vector2i& position);
            void SetPosition(uint32_t x, uint32_t y);
            void SetPositionX(uint32_t x);
            void SetPositionY(uint32_t y);

            void SetDimensions(const math::Vector2i& dimensions);
            void SetDimensions(uint32_t width, uint32_t height);
            void SetWidth(uint32_t width);
            void SetHeight(uint32_t height);

            math::Vector2i GetDimensions() const;
            uint32_t GetWidth() const;
            uint32_t GetHeight() const;

            math::Vector2i GetAbsolutePosition() const;

            bool AddChild(Element* pChildElement);
            bool RemoveChild(Element* pChildElement); // Not recursive!

            Element* GetParent();

        protected:
            // Called by Draw(). Implement any specific drawing logic for this element type
            // here that is not common to all elements.
            virtual void DrawInternal() = 0;

            // Same deal here
            virtual void TickInternal(float dt) = 0;

            void SignalRootElement(const Event& event);

        private:
            Element* m_pParent = nullptr;

            uint32_t m_positionX = 0;
            uint32_t m_positionY = 0;

            uint32_t m_width = 0;
            uint32_t m_height = 0;

            // Increment as needed
            static const uint32_t kMaxChildren = 4;
            Element* m_children[kMaxChildren];
            uint32_t m_numChildren = 0;

            std::deque<Event> m_eventQueue;
        };
    }
}
