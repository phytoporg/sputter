#include "uniform.h"
#include "render.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace sputter::render;

#define UNIFORM_IMPL(glFunc, tType, dType) \
    template<> void Uniform<tType>::Set(uint32_t slot, const tType* data, uint32_t length) \
    { \
        glFunc(slot, static_cast<GLsizei>(length), reinterpret_cast<const dType*>(data)); \
    }

template Uniform<int>;
template Uniform<glm::ivec2>;
template Uniform<glm::ivec4>;
template Uniform<float>;
template Uniform<glm::vec2>;
template Uniform<glm::vec3>;
template Uniform<glm::vec4>;
template Uniform<glm::quat>;
template Uniform<glm::mat4>;

UNIFORM_IMPL(glUniform1iv, int, int);
UNIFORM_IMPL(glUniform2iv, glm::ivec2, int);
UNIFORM_IMPL(glUniform4iv, glm::ivec4, int);
UNIFORM_IMPL(glUniform1fv, float, float);
UNIFORM_IMPL(glUniform1fv, glm::vec2, float);
UNIFORM_IMPL(glUniform1fv, glm::vec3, float);
UNIFORM_IMPL(glUniform1fv, glm::vec4, float);
UNIFORM_IMPL(glUniform1fv, glm::quat, float);

template<> 
void Uniform<glm::mat4>::Set(uint32_t slot, const glm::mat4* data, uint32_t length)
{
    glUniformMatrix4fv(slot, static_cast<GLsizei>(length), false, reinterpret_cast<const float*>(data));
}