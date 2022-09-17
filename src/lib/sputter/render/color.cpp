#include "color.h"

using namespace sputter::render;

glm::vec3 Color::ToVec3() const
{
    return glm::vec3(Red / 255.f, Green / 255.f, Blue / 255.f);
}