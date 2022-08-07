#include "shaderstorage.h"
#include "render.h"

#include <sputter/assets/textdata.h>
#include <sputter/system/system.h>

#include <algorithm>

using namespace sputter::render;

ShaderStorage::~ShaderStorage()
{
    while (!m_storageVector.empty())
    {
        ReleaseResource(m_storageVector.front().get());
    }
}

bool
ShaderStorage::AddShader(
    const assets::TextData& vertexText,
    const assets::TextData& fragmentText,
    const std::string& shaderName
    )
{
    const std::string vertexShaderSource = vertexText.TextStream.str();
    if (vertexShaderSource.empty())
    {
        LOG(ERROR) << "Could not load vertex shader for " << shaderName << std::endl;
        return false;
    }

    const std::string fragmentShaderSource = fragmentText.TextStream.str();
    if (fragmentShaderSource.empty())
    {
        LOG(ERROR) << "Could not load fragment shader for " << shaderName << std::endl;
        return false;
    }

    const uint32_t vertexShaderHandle = CompileVertexShader(vertexShaderSource);
    if (!vertexShaderHandle) { return false; }

    const uint32_t fragmentShaderHandle = CompileFragmentShader(fragmentShaderSource);
    if (!fragmentShaderHandle) { return false; }

    const uint32_t programHandle = glCreateProgram();
    if (!LinkShaders(programHandle, vertexShaderHandle, fragmentShaderHandle))
    {
        LOG(ERROR) << "Failed to link shader " << shaderName;
        return false;
    }

    const std::vector<ShaderAttribute> attributes = PopulateAttributes(programHandle);
    const std::vector<ShaderUniform> uniforms = PopulateUniforms(programHandle);

    Shader* pShader = new Shader(programHandle, attributes, uniforms, shaderName);
    if (!pShader)
    {
        LOG(ERROR) << "Failed to allocate shader. OOM?" << std::endl;
        return false;
    }

    if (!AddResource(pShader))
    {
        delete pShader;
        return false;
    }
    
    return true;
}

ShaderPtr ShaderStorage::FindShaderByName(const std::string& shaderName) const 
{
    for (const ShaderPtr& spShader : m_storageVector)
    {
        if (spShader->GetName() == shaderName)
        {
            return spShader;
        }
    }

    return nullptr;
}

uint32_t ShaderStorage::CompileVertexShader(const std::string& source) 
{
    uint32_t shaderHandle = glCreateShader(GL_VERTEX_SHADER);
    const char* pSource = source.c_str();
    glShaderSource(shaderHandle, 1, &pSource, NULL);
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

uint32_t ShaderStorage::CompileFragmentShader(const std::string& source) 
{
    uint32_t shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
    const char* pSource = source.c_str();
    glShaderSource(shaderHandle, 1, &pSource, NULL);
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

bool ShaderStorage::LinkShaders(uint32_t programHandle, uint32_t vertexShaderHandle, uint32_t fragmentShaderHandle) 
{
    if (vertexShaderHandle <= 0 || fragmentShaderHandle <= 0)
    {
        LOG(ERROR) << "Invalid shader handle passed to LinkShaders()" << std::endl;
        return false;
    }

    glAttachShader(programHandle, vertexShaderHandle);
    glAttachShader(programHandle, fragmentShaderHandle);
    glLinkProgram(programHandle);

    int success = 0;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(programHandle, sizeof(infoLog), NULL, infoLog);
        LOG(ERROR) << "Failed to link shader" << "\t" << infoLog << std::endl;
        glDeleteShader(vertexShaderHandle);
        glDeleteShader(fragmentShaderHandle);
        return false;
    }

    glDeleteShader(vertexShaderHandle);
    glDeleteShader(fragmentShaderHandle);
    return true;   
}

std::vector<ShaderAttribute> ShaderStorage::PopulateAttributes(uint32_t programHandle) 
{
    int count = -1;
    int length = -1;
    int size = -1;
    char name[128];
    GLenum type;

    glUseProgram(programHandle);
    glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &count);

    std::vector<ShaderAttribute> attributes;
    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(name));
        glGetActiveAttrib(programHandle, static_cast<GLuint>(i), sizeof(name), &length, &size, &type, name);
        const int attributeLocation = glGetAttribLocation(programHandle, name);
        if (attributeLocation >= 0)
        {
            attributes.emplace_back(name, attributeLocation);
        }
    }

    glUseProgram(0);
    return attributes;
}

std::vector<ShaderUniform> ShaderStorage::PopulateUniforms(uint32_t programHandle) 
{
    int count = -1;
    int length = -1;
    int size = -1;
    char name[128];
    GLenum type;

    glUseProgram(programHandle);
    glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &count);

    std::vector<ShaderUniform> uniforms;
    for (int i = 0; i < count; ++i)
    {
        memset(name, 0, sizeof(name));
        glGetActiveUniform(programHandle, static_cast<GLuint>(i), sizeof(name), &length, &size, &type, name);
        const int uniformLocation = glGetUniformLocation(programHandle, name);
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

                    uniformLocation = glGetUniformLocation(programHandle, uniformArrayElementName);
                    if (uniformLocation >= 0)
                    {
                        uniforms.emplace_back(uniformArrayElementName, uniformLocation);
                    }
                    else
                    {
                        // Reached the end of the uniform array.
                        break;
                    }
                }
            }
            uniforms.emplace_back(uniformName, uniformLocation);
        }
    }

    glUseProgram(0);
    return uniforms;
}

bool ShaderStorage::AddResource(Shader* pShader)
{
    if (!pShader) { return false; }

    if (FindShaderByName(pShader->GetName()))
    {
        LOG(WARNING) << "Attempted to add duplicate shader: " << pShader->GetName();
        return false;
    }
     
    m_storageVector.emplace_back(pShader);
    return true;
}

bool ShaderStorage::ReleaseResource(Shader* pShader)
{        
    auto it = std::find_if(
        std::begin(m_storageVector),
        std::end(m_storageVector),
        [pShader](const ShaderPtr& spShader) { return spShader.get() == pShader; });
    if (it != std::end(m_storageVector))
    {
        m_storageVector.erase(it);
        return true;
    }
        
    LOG(WARNING) << "Failed to remove shader " << pShader->GetName() 
                 << " from storage: could not locate in storage.";
    return false;
}