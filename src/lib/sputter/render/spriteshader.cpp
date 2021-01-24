#include "spriteshader.h"

#include <vector>

#include <GL/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include <sputter/system/system.h>

namespace {
    const char* VertexShaderSource   = R"(
        #version 430
        // <vec3 position, vec2 texCoords>
        layout (location = 0) in vec3 vertex; 
        layout (location = 1) in vec2 texCoords; 

        out vec2 TexCoords;

        uniform mat4 model;
        uniform mat4 projection;

        void main()
        {
            TexCoords = texCoords;
            gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
        })";

    const char* FragmentShaderSource = R"(
        #version 430
        in vec2 TexCoords;
        out vec4 color;

        uniform sampler2D image;

        void main()
        {    
            //color = texture(image, TexCoords);
            // color = vec4(1.0, 0.0, 0.0, 1.0);
            // Something more interesting while we debug textures
            color = vec4(
                gl_FragCoord.x / 1024.0,
                gl_FragCoord.y / 768.0,
                0.0, 1.0);
        })";
}

namespace sputter { namespace render {
    SpriteShader::SpriteShader()
    {
        m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_vertexShaderId, 1, &VertexShaderSource, nullptr);
        glCompileShader(m_vertexShaderId);

        {
            GLint success;
            glGetShaderiv(m_vertexShaderId, GL_COMPILE_STATUS, &success);

            if (success == GL_FALSE)
            {
                GLint logSize = 0;
                glGetShaderiv(m_vertexShaderId, GL_INFO_LOG_LENGTH, &logSize);

                std::vector<char> logData(logSize);
                glGetShaderInfoLog(
                    m_vertexShaderId,
                    logSize,
                    nullptr,
                    logData.data());

                system::LogAndFail(
                    "Failed to compile sprite vertex shader: " +
                    std::string(std::begin(logData), std::end(logData)));
            }
        }
        
        m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_fragmentShaderId, 1, &FragmentShaderSource, nullptr);
        glCompileShader(m_fragmentShaderId);

        GLint success;
        glGetShaderiv(m_fragmentShaderId, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint logSize = 0;
            glGetShaderiv(m_fragmentShaderId, GL_INFO_LOG_LENGTH, &logSize);

            std::vector<char> logData(logSize);
            glGetShaderInfoLog(
                m_fragmentShaderId,
                logSize,
                nullptr,
                logData.data());

            system::LogAndFail(
                "Failed to compile sprite fragment shader: " +
                std::string(std::begin(logData), std::end(logData)));
        }

        m_programId = glCreateProgram();

        glAttachShader(m_programId, m_vertexShaderId);
        glAttachShader(m_programId, m_fragmentShaderId);

        glLinkProgram(m_programId);

        GLint isLinked = 0;
        glGetProgramiv(m_programId, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint logSize = 0;
            glGetProgramiv(m_fragmentShaderId, GL_INFO_LOG_LENGTH, &logSize);

            std::vector<char> logData(logSize);
            glGetProgramInfoLog(
                m_programId,
                logSize,
                nullptr,
                logData.data());

            system::LogAndFail(
                "Failed to link sprite shader program: " +
                std::string(std::begin(logData), std::end(logData)));
        }

        glDetachShader(m_programId, m_vertexShaderId);
        glDetachShader(m_programId, m_fragmentShaderId);

        m_uniformModelId = glGetUniformLocation(m_programId, "model");
        m_uniformProjId  = glGetUniformLocation(m_programId, "projection");
        m_uniformTexId   = glGetUniformLocation(m_programId, "image");
    }

    SpriteShader::~SpriteShader()
    {
        glDeleteProgram(m_programId);
        glDeleteShader(m_vertexShaderId);
        glDeleteShader(m_fragmentShaderId);
    }

    void SpriteShader::SetUniformModelMatrix(const glm::mat4& m)
    {
        glUniformMatrix4fv(m_uniformModelId, 1, GL_FALSE, glm::value_ptr(m));
    }
    
    void SpriteShader::SetUniformProjMatrix(const glm::mat4& m)
    {
        glUniformMatrix4fv(m_uniformProjId, 1, GL_FALSE, glm::value_ptr(m));
    }

    void SpriteShader::SetUniformTextureId(int32_t textureId)
    {
        glUniform1i(m_uniformTexId, 0);
    }

    void SpriteShader::Use() const
    {
        glUseProgram(m_programId);
    }
}}
