#include "drawshapes.h"
#include "geometry.h"
#include "shaderstorage.h"
#include "render.h"
#include "attribute.h"
#include "uniform.h"
#include "draw.h"
#include "indexbuffer.h"

#include <vector>
#include <string>

#include <sputter/system/system.h>

using namespace sputter::render;

static bool IsInitialized = false;

static const char* kLineFragmentShaderName = "line_frag";
static const char* kLineVertexShaderName = "line_vert";
static const char* kLineShaderName = "line";
static Shader* pLineShader = nullptr;

static glm::mat4 ViewMatrix;
static glm::mat4 ProjectionMatrix;

static const char* pViewUniformName = "view";
static uint32_t ViewMatrixUniformHandle = Shader::kInvalidHandleValue;

static const char* pProjectionUniformName = "proj";
static uint32_t ProjectionMatrixUniformHandle = Shader::kInvalidHandleValue;

static const char* pBorderColorUniformName = "color";
static uint32_t BorderColorUniformHandle = Shader::kInvalidHandleValue;

static uint32_t VAO = 0;

static const uint32_t kMaxVertices = 16; // Increase if needed
static std::vector<glm::ivec2> VertexPositions;
static Attribute<glm::ivec2>* pVertexPositionAttribute = nullptr;

static const uint32_t kMaxIndices = 32; // Increase if needed
static std::vector<uint32_t> VertexIndices;
static IndexBuffer* pIndices = nullptr;

bool shapes::InitializeLineRenderer(assets::AssetStorage* pAssetStorage, ShaderStorage* pShaderStorage, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    // Do not allow for reinit. If any parameters need to be updated, address by adding accessors as needed.
    if (IsInitialized)
    {
        LOG(WARNING) << "InitializeLineRenderer() called more than once";
        return false;
    }

    if (!pShaderStorage->AddShaderFromShaderAssetNames(
        pAssetStorage,
        kLineVertexShaderName,
        kLineFragmentShaderName,
        kLineShaderName))
    {
        LOG(ERROR) << "Failed to load line shader";
        return false;
    }
    
    ShaderPtr spShader = pShaderStorage->FindShaderByName(kLineShaderName);
    pLineShader = spShader.get();
    ViewMatrixUniformHandle = pLineShader->GetUniform(pViewUniformName);
    if (ViewMatrixUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(WARNING) << "Failed to reterieve view matrix uniform handle for line renderer.";
        return false;
    }

    ProjectionMatrixUniformHandle = pLineShader->GetUniform(pProjectionUniformName);
    if (ProjectionMatrixUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(WARNING) << "Failed to reterieve projection matrix uniform handle for line renderer.";
        return false;
    }

    BorderColorUniformHandle = pLineShader->GetUniform(pBorderColorUniformName);
    if (BorderColorUniformHandle == Shader::kInvalidHandleValue)
    {
        LOG(WARNING) << "Failed to reterieve border color uniform handle for line renderer.";
        return false;
    }

    ViewMatrix = viewMatrix;
    ProjectionMatrix = projectionMatrix;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    VertexPositions.reserve(kMaxVertices);

    pVertexPositionAttribute = new Attribute<glm::ivec2>();
    pVertexPositionAttribute->Set(VertexPositions.data(), VertexPositions.size());
    pVertexPositionAttribute->BindTo(0);

    VertexIndices.reserve(kMaxIndices);

    pIndices = new IndexBuffer();
    pIndices->Set(VertexIndices.data(), VertexIndices.size());

    glBindVertexArray(0);

    IsInitialized = true;
    return true;
}

void shapes::UninitializeLineRenderer()
{
    if (!IsInitialized)
    {
        LOG(WARNING) << "Attempting to uninitialize uninitialized line renderer";
        return;
    }
    
    delete pIndices;
    pIndices = nullptr;

    delete pVertexPositionAttribute;
    pVertexPositionAttribute = nullptr;

    pLineShader = nullptr;

    ViewMatrixUniformHandle = Shader::kInvalidHandleValue;
    ProjectionMatrixUniformHandle = Shader::kInvalidHandleValue;
    BorderColorUniformHandle = Shader::kInvalidHandleValue;

    glDeleteBuffers(1, &VAO);
    VAO = 0;

    VertexPositions.clear();
    VertexIndices.clear();

    IsInitialized = false;
}

// TODO: This performs a draw call per invocation-- once this is working, maybe RECTify that (ha ha)
void shapes::DrawRect(
    int32_t x, int32_t y, int32_t width, int32_t height, int32_t borderSize, const Color& borderColor)
{
    if (!IsInitialized)
    {
        system::LogAndFail("Line renderer not initialized in DrawRect()");
    }

    pLineShader->Bind();

    Uniform<glm::mat4>::Set(ViewMatrixUniformHandle, ViewMatrix);
    Uniform<glm::mat4>::Set(ProjectionMatrixUniformHandle, ProjectionMatrix);

    const glm::vec3& Vec3BorderColor = borderColor.ToVec3();
    Uniform<glm::vec3>::Set(BorderColorUniformHandle, Vec3BorderColor);

    glBindVertexArray(VAO);
    pVertexPositionAttribute->BindTo(0);

    pVertexPositionAttribute->Set(VertexPositions);
    ::Draw(*pIndices, DrawMode::Triangles);

    pVertexPositionAttribute->UnbindFrom(0);

    pLineShader->Unbind();
    glBindVertexArray(0);
}