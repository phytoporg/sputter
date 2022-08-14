#include "uniform.h"
#include "render.h"

#include <sputter/system/system.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace sputter::render;

void sputter::render::SetUniformByType(uint32_t slot, UniformType type, void const* pValue)
{
    switch (type)
    {
        case UniformType::Int:
            Uniform<int>::Set(slot, *static_cast<const int*>(pValue));
            break;
        case UniformType::IVec2:
            Uniform<glm::ivec2>::Set(slot, *static_cast<const glm::ivec2*>(pValue));
            break;
        case UniformType::IVec4:
            Uniform<glm::ivec4>::Set(slot, *static_cast<const glm::ivec4*>(pValue));
            break;
        case UniformType::Float:
            Uniform<float>::Set(slot, *static_cast<const float*>(pValue));
            break;
        case UniformType::Vec2:
            Uniform<glm::vec2>::Set(slot, *static_cast<const glm::vec2*>(pValue));
            break;
        case UniformType::Vec3:
            Uniform<glm::vec3>::Set(slot, *static_cast<const glm::vec3*>(pValue));
            break;
        case UniformType::Quat:
            Uniform<glm::quat>::Set(slot, *static_cast<const glm::quat*>(pValue));
            break;
        case UniformType::Mat4:
            Uniform<glm::mat4>::Set(slot, *static_cast<const glm::mat4*>(pValue));
            break;
        default:
            sputter::system::LogAndFail("Unexpected uniform type!");
    }
}

#define UNIFORM_IMPL(glFunc, tType, dType) \
    template<> void Uniform<tType>::Set(uint32_t slot, const tType* data, uint32_t length) \
    { \
        glFunc(slot, static_cast<GLsizei>(length), reinterpret_cast<const dType*>(data)); \
    }

// This apparently accomplishes nothing
// TODO: Make sure MSVC is happy, then this can be removed
// template Uniform<int>;
// template Uniform<glm::ivec2>;
// template Uniform<glm::ivec4>;
// template Uniform<float>;
// template Uniform<glm::vec2>;
// template Uniform<glm::vec3>;
// template Uniform<glm::vec4>;
// template Uniform<glm::quat>;
// template Uniform<glm::mat4>;

UNIFORM_IMPL(glUniform1iv, int, int);
UNIFORM_IMPL(glUniform2iv, glm::ivec2, int);
UNIFORM_IMPL(glUniform4iv, glm::ivec4, int);
UNIFORM_IMPL(glUniform1fv, float, float);
UNIFORM_IMPL(glUniform2fv, glm::vec2, float);
UNIFORM_IMPL(glUniform3fv, glm::vec3, float);
UNIFORM_IMPL(glUniform4fv, glm::vec4, float);
UNIFORM_IMPL(glUniform4fv, glm::quat, float);

template<> 
void Uniform<glm::mat4>::Set(uint32_t slot, const glm::mat4* data, uint32_t length)
{
    glUniformMatrix4fv(slot, static_cast<GLsizei>(length), false, reinterpret_cast<const float*>(data));
}