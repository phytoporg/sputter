#include "attribute.h"
#include "render.h"
#include <glm/glm.hpp>

using namespace sputter::render;

template Attribute<int>;
template<>
void Attribute<int>::SetAttributePointer(uint32_t slot)
{
    glVertexAttribIPointer(slot, 1, GL_INT, 0, nullptr);
}

template Attribute<glm::ivec4>;
void Attribute<glm::ivec4>::SetAttributePointer(uint32_t slot)
{
    glVertexAttribIPointer(slot, 4, GL_INT, 0, nullptr);
}

template Attribute<float>;
void Attribute<float>::SetAttributePointer(uint32_t slot)
{
    glVertexAttribPointer(slot, 1, GL_FLOAT, GL_FALSE, 0, 0);
}

template Attribute<glm::vec2>;
void Attribute<glm::vec2>::SetAttributePointer(uint32_t slot)
{
    glVertexAttribPointer(slot, 2, GL_FLOAT, GL_FALSE, 0, 0);
}


template Attribute<glm::vec3>;
void Attribute<glm::vec3>::SetAttributePointer(uint32_t slot)
{
    glVertexAttribPointer(slot, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

template Attribute<glm::vec4>;
void Attribute<glm::vec4>::SetAttributePointer(uint32_t slot)
{
    glVertexAttribPointer(slot, 4, GL_FLOAT, GL_FALSE, 0, 0);
}
