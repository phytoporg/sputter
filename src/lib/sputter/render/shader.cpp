#include "shader.h"
#include "render.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <sputter/system/system.h>

using namespace sputter::render;

const uint32_t sputter::render::Shader::kInvalidHandleValue = static_cast<uint32_t>(-1);

Shader::Shader()
    : m_handle(glCreateProgram())
{}

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    : Shader()
{
    Load(vertexShaderFilePath, fragmentShaderFilePath);
}

Shader::~Shader()
{
    glDeleteProgram(m_handle);
}

void Shader::Load(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
    const std::string vertexShaderSource = ReadFile(vertexShaderFilePath);
    if (vertexShaderSource.empty())
    {
        LOG(ERROR) << "Could not load vertex shader at " << vertexShaderFilePath << std::endl;
        return;
    }

    const std::string fragmentShaderSource = ReadFile(fragmentShaderFilePath);
    if (fragmentShaderSource.empty())
    {
        LOG(ERROR) << "Could not load fragment shader at " << fragmentShaderFilePath << std::endl;
        return;
    }

    const uint32_t vertexShaderHandle = CompileVertexShader(vertexShaderSource);
    if (!vertexShaderHandle) { return;  }

    const uint32_t fragmentShaderHandle = CompileFragmentShader(fragmentShaderSource);
    if (!fragmentShaderHandle) { return; }

    if (LinkShaders(vertexShaderHandle, fragmentShaderHandle))
    {
        PopulateAttributes();
        PopulateUniforms();
    }
}

void Shader::Bind()
{
    // TODO
}

void Shader::Unbind()
{
    // TODO
}

uint32_t Shader::GetAttribute(const std::string& name) const
{
    // TODO
    return kInvalidHandleValue;
}

uint32_t Shader::GetUniform(const std::string& name) const
{
    // TODO
    return kInvalidHandleValue;
}

uint32_t Shader::GetHandle() const
{
    return m_handle;
}

std::string Shader::ReadFile(const std::string& filePath)
{
    static const std::string emptyFile;

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        return emptyFile;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    
    return ss.str();
}

uint32_t Shader::CompileVertexShader(const std::string& shaderText)
{
    uint32_t shaderHandle = glCreateShader(GL_VERTEX_SHADER);
    const char* shaderSource = shaderText.c_str();
    glShaderSource(shaderHandle, 1, &shaderSource, NULL);
    glCompileShader(shaderHandle);

    int success = 0;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderHandle, sizeof(infoLog), NULL, infoLog);
        LOG(ERROR) << "Vertex shader compilation failed\n"
                   << "\t" << infoLog << std::endl;
        glDeleteProgram(shaderHandle);
        return 0;
    }

    return shaderHandle;
}

uint32_t Shader::CompileFragmentShader(const std::string& shaderText)
{
    // TODO: Gross, kinda copy-and-paste-y. Generalize some other day.
    uint32_t shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    const char* shaderSource = shaderText.c_str();
    glShaderSource(shaderHandle, 1, &shaderSource, NULL);
    glCompileShader(shaderHandle);

    int success = 0;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderHandle, sizeof(infoLog), NULL, infoLog);
        LOG(ERROR) << "Fragment shader compilation failed\n"
                   << "\t" << infoLog << std::endl;
        glDeleteProgram(shaderHandle);
        return 0;
    }

    return shaderHandle;
}

bool Shader::LinkShaders(uint32_t vertexShaderHandle, uint32_t fragmentShaderHandle)
{
    if (vertexShaderHandle <= 0 || fragmentShaderHandle <= 0)
    {
        LOG(ERROR) << "Invalid shader handle passed to Shader::LinkShaders()" << std::endl;
        return false;
    }

    glAttachShader(m_handle, vertexShaderHandle);
    glAttachShader(m_handle, fragmentShaderHandle);
    glLinkProgram(m_handle);

    int success = 0;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_handle, sizeof(infoLog), NULL, infoLog);
        LOG(ERROR) << "Failed to link shader" << "\t" << infoLog << std::endl;
        glDeleteShader(vertexShaderHandle);
        glDeleteShader(fragmentShaderHandle);
        return false;
    }

    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    return true;
}

void Shader::PopulateAttributes()
{
    int count = -1;
    int length = -1;
    int size = -1;
    char name[128];
    GLenum type;

    glUseProgram(m_handle);
    glGetProgramiv(m_handle, GL_ACTIVE_ATTRIBUTES, &count);

    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(name));
        glGetActiveAttrib(m_handle, static_cast<GLuint>(i), sizeof(name), &length, &size, &type, name);
        const int attributeLocation = glGetAttribLocation(m_handle, name);
        if (attributeLocation >= 0)
        {
            m_attributes.emplace_back(name, attributeLocation);
        }
    }

    glUseProgram(0);
}

void Shader::PopulateUniforms()
{
    int count = -1;
    int length = -1;
    int size = -1;
    char name[128];
    GLenum type;

    glUseProgram(m_handle);
    glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, &count);

    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(name));
        glGetActiveAttrib(m_handle, static_cast<GLuint>(i), sizeof(name), &length, &size, &type, name);
        const int uniformLocation = glGetAttribLocation(m_handle, name);
        if (uniformLocation >= 0)
        {
            std::string uniformName = name;
            const size_t leftBracketIndex = uniformName.find('[');
            if (leftBracketIndex != std::string::npos)
            {
                // This uniform is an array. Append [index] for every valid index in the uniform to
                // build the name;

                char uniformArrayElementName[256];
                uniformName.erase(std::begin(uniformName) + leftBracketIndex, std::end(uniformName));
                uint32_t uniformIndex = 0;
                uint32_t uniformLocation = 0;

                while(true)
                {
                    memset(uniformArrayElementName, 0, sizeof(uniformArrayElementName));
                    sprintf(uniformArrayElementName, "%s[%d]", uniformName.c_str(), uniformIndex++);

                    uniformLocation = glGetUniformLocation(m_handle, uniformArrayElementName);
                    if (uniformLocation >= 0)
                    {
                        m_uniforms.emplace_back(uniformArrayElementName, uniformLocation);
                    }
                    else
                    {
                        // Reached the end of the uniform array.
                        break;
                    }
                }
            }
            m_uniforms.emplace_back(uniformName, uniformLocation);
        }
    }

    glUseProgram(0);
}

