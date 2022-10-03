#include "color.h"

using namespace sputter::render;

Color Color::RED       = Color{255, 0, 0};
Color Color::DARKRED   = Color{140, 0, 0};
Color Color::BLUE      = Color{0, 0, 255};
Color Color::GREEN     = Color{0, 255, 0};
Color Color::WHITE     = Color{255, 255, 255};
Color Color::BLACK     = Color{0, 0, 0};
Color Color::GRAY      = Color{125, 125, 125};

glm::vec3 Color::ToVec3() const
{
    return glm::vec3(R / 255.f, G / 255.f, B / 255.f);
}
