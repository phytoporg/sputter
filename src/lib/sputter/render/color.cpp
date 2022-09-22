#include "color.h"

using namespace sputter::render;

Color Color::Red   = Color{255, 0, 0};
Color Color::Blue  = Color{0, 255, 0};
Color Color::Green = Color{0, 0, 255};
Color Color::White = Color{255, 255, 255};
Color Color::Gray = Color{125, 125, 125};

glm::vec3 Color::ToVec3() const
{
    return glm::vec3(R / 255.f, G / 255.f, B / 255.f);
}
