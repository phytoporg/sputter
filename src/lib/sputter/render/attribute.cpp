#include "attribute.h"
#include "render.h"

using sputter::render;

template Attribute<int>;
template Attribute<float>;
template Attribute<glm::vec2>;
template Attribute<glm::vec3>;
template Attribute<glm::vec4>;
template Attribute<glm::ivec4>;
