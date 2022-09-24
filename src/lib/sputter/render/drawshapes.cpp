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

#include <sputter/math/vector2i.h>
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

static const char* pProjectionUniformName = "projection";
static uint32_t ProjectionMatrixUniformHandle = Shader::kInvalidHandleValue;

static uint32_t VAO = 0;

static const uint32_t kMaxVertices = 64; // Increase if needed
static std::vector<glm::ivec2> VertexPositions;
static Attribute<glm::ivec2>* pVertexPositionAttribute = nullptr;

static std::vector<glm::vec3> VertexColors;
static Attribute<glm::vec3>*  pVertexColorAttribute = nullptr;

static const uint32_t kMaxIndices = 128; // Increase if needed
static std::vector<int> VertexIndices;
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

    ViewMatrix = viewMatrix;
    ProjectionMatrix = projectionMatrix;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    VertexPositions.reserve(kMaxVertices);
    VertexColors.reserve(kMaxVertices);

    pVertexPositionAttribute = new Attribute<glm::ivec2>();
    pVertexPositionAttribute->Set(VertexPositions.data(), VertexPositions.size());
    pVertexPositionAttribute->BindTo(0);

    pVertexColorAttribute = new Attribute<glm::vec3>();
    pVertexColorAttribute->Set(VertexColors.data(), VertexColors.size());
    pVertexColorAttribute->BindTo(1);

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

    delete pVertexColorAttribute;
    pVertexColorAttribute = nullptr;

    pLineShader = nullptr;

    ViewMatrixUniformHandle = Shader::kInvalidHandleValue;
    ProjectionMatrixUniformHandle = Shader::kInvalidHandleValue;

    glDeleteBuffers(1, &VAO);
    VAO = 0;

    VertexPositions.clear();
    VertexColors.clear();
    VertexIndices.clear();

    IsInitialized = false;
}

void shapes::FlushLineRenderer()
{
    if (!IsInitialized)
    {
        system::LogAndFail("Line rendere is not initialized in FlushLineRenderer()");
    }

    if (VertexPositions.empty())
    {
        // Nothing to do
        return;
    }

    pLineShader->Bind();

    Uniform<glm::mat4>::Set(ViewMatrixUniformHandle, ViewMatrix);
    Uniform<glm::mat4>::Set(ProjectionMatrixUniformHandle, ProjectionMatrix);

    glBindVertexArray(VAO);
    pVertexPositionAttribute->BindTo(0);
    pVertexColorAttribute->BindTo(1);

    pVertexPositionAttribute->Set(VertexPositions.data(), VertexPositions.size());
    pVertexColorAttribute->Set(VertexColors.data(), VertexColors.size());
    pIndices->Set(VertexIndices.data(), VertexIndices.size());

    ::Draw(*pIndices, DrawMode::Triangles);

    pVertexPositionAttribute->UnbindFrom(0);
    pVertexColorAttribute->UnbindFrom(1);

    pLineShader->Unbind();
    glBindVertexArray(0);

    // Reset data for the next draw call
    VertexPositions.clear(); // Do we need to call resize(0) instead?
    VertexColors.clear();
    VertexIndices.clear();
}

// TODO: This performs a draw call per invocation-- once this is working, maybe RECTify that (ha ha)
void shapes::DrawRect(
    int32_t x, int32_t y, int32_t width, int32_t height, int32_t borderSize, const Color& borderColor)
{
    if (!IsInitialized)
    {
        system::LogAndFail("Line renderer not initialized in DrawRect()");
    }

    const uint32_t NumVertices = 8;
    if (VertexPositions.size() + NumVertices > kMaxVertices)
    {
        system::LogAndFail("Attempting to draw too many vertices. Reconsider kMaxVertices.");
    }
    
    const uint32_t PreviousNumPositions = VertexPositions.size();
    VertexPositions.resize(VertexPositions.size() + NumVertices);
    VertexColors.resize(VertexColors.size() + NumVertices);

    if (VertexPositions.size( )!= VertexColors.size())
    {
        system::LogAndFail("Color and position vectors should be the same size.");
    }

    const uint32_t NumIndices = 24;
    if (VertexIndices.size() + NumIndices > kMaxIndices)
    {
        system::LogAndFail("Attempting to draw with too many index values. Reconsider kMaxIndices.");
    }
    const uint32_t PreviousNumIndices = VertexIndices.size();
    VertexIndices.resize(VertexIndices.size() + NumIndices);
    geometry::MakeBorderedRect(x, y, width, height, borderSize, &VertexPositions[PreviousNumPositions], NumVertices, &VertexIndices[PreviousNumIndices], NumIndices);

    // Offset the indices by PrevNumPositions
    for (uint32_t i = PreviousNumIndices; i < VertexIndices.size(); ++i)
    {
        VertexIndices[i] += PreviousNumPositions;
    }

    // Set up the color elements
    for (uint32_t i = PreviousNumPositions; i < VertexPositions.size(); ++i)
    {
        VertexColors[i] = borderColor.ToVec3();
    }
}

void shapes::DrawRect(const math::Vector2i& position, const math::Vector2i& dimensions, int32_t borderSize, const Color& borderColor)
{
    DrawRect(position.GetX(), position.GetY(), dimensions.GetX(), dimensions.GetY(), borderSize, borderColor);
}