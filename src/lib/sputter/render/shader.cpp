#include "shader.h"
#include "render.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <sputter/system/system.h>

using namespace sputter::render;

const uint32_t Shader::kInvalidHandleValue = static_cast<uint32_t>(-1);

Shader::Shader(
    uint32_t programHandle,
    const std::vector<ShaderAttribute>& attributes,
    const std::vector<ShaderUniform>& uniforms,
    const std::string& name
    ) : m_handle(programHandle), m_attributes(attributes), m_uniforms(uniforms), m_name(name)
{}

Shader::~Shader()
{
    glDeleteProgram(m_handle);
}

void Shader::Bind()
{
    glUseProgram(m_handle);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

uint32_t Shader::GetAttribute(const std::string& name) const
{
    auto it = std::find_if(std::begin(m_attributes), std::end(m_attributes),
        [&name](const ShaderAttribute& attribute) { return  name == attribute.Name; });
    if (it != std::end(m_attributes))
    {
        return it->Location;
    }

    return kInvalidHandleValue;
}

uint32_t Shader::GetUniform(const std::string& name) const
{
    auto it = std::find_if(std::begin(m_uniforms), std::end(m_uniforms),
        [&name](const ShaderUniform& uniform) { return  name == uniform.Name; });
    if (it != std::end(m_uniforms))
    {
        return it->Location;
    }

    return kInvalidHandleValue;
}

uint32_t Shader::GetHandle() const
{
    return m_handle;
}

const std::string& Shader::GetName() const
{
    return m_name;
}
