#pragma once

#include <cstdint>

namespace sputter 
{
    namespace ui
    {
        class Element
        {
        public:
            // For root elements with no parent
            Element();
            Element(Element* pParent);

            virtual ~Element();

            virtual void Tick(float dt) = 0;
            virtual void Draw() = 0;

            virtual void HandleEvent(uint32_t eventCode, void* pEventData) = 0;

            // These positions are relative to the parent element
            void SetPosition(uint32_t x, uint32_t y);
            void SetPositionX(uint32_t x);
            void SetPositionY(uint32_t y);

            void SetDimensions(uint32_t width, uint32_t height);
            void SetWidth(uint32_t width);
            void SetHeight(uint32_t height);

        private:
            Element* m_pParent = nullptr;

            uint32_t m_positionX = 0;
            uint32_t m_positionY = 0;

            uint32_t m_width = 0;
            uint32_t m_height = 0;
        };
    }
}