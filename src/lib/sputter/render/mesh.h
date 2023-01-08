#pragma once

#include "shader.h"
#include "uniform.h"
#include "meshconstants.h"
#include <cstring>
#include <initializer_list>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>
#include <sputter/containers/fixedmemoryvector.h>
#include <sputter/containers/arrayvector.h>
#include <sputter/game/subsystemtype.h>
#include <sputter/render/attribute.h>
#include <sputter/render/indexbuffer.h>

namespace sputter { namespace render {
    class Shader;

    struct MeshUniformValue
    {
        MeshUniformValue() {}

        MeshUniformValue(const char* pUniformName, UniformType type, void const* pValue)
        : Type(type), pValue(pValue) 
        {
            strncpy(Name, pUniformName, sizeof(Name));
        }

        MeshUniformValue(const MeshUniformValue& other)
        {
            memcpy(this, &other, sizeof(MeshUniformValue));
        }

        MeshUniformValue operator=(const MeshUniformValue& other)
        {
            memcpy(this, &other, sizeof(MeshUniformValue));
            return *this;
        }

        char Name[256] = {};
        UniformType Type = UniformType::Invalid;
        void const* pValue = nullptr;

        // TO cache the name -> slot lookups
        uint32_t Location = Shader::kInvalidHandleValue;
    };

    class Mesh 
    {
        public:
            static const sputter::game::SubsystemType ComponentId = 
                sputter::game::SubsystemType::TYPE_MESH;

            struct InitializationParameters {};

            Mesh();
            Mesh(size_t maxVertexCount, size_t maxIndexCount);
            Mesh(const Mesh& other);
            Mesh& operator=(const Mesh& other);

            bool SetPositions(
                const sputter::containers::FixedMemoryVector<sputter::math::FPVector3D>& vertexPositions
                );
            bool SetPositions(
                const sputter::math::FPVector3D* positionArray, uint32_t arrayLen
                );

            bool SetNormals(
                const sputter::containers::FixedMemoryVector<sputter::math::FPVector3D>& vertexNormals
                );
            bool SetNormals(
                const sputter::math::FPVector3D* normalsArray, uint32_t arrayLen
                );

            bool SetTextureCoordinates(
                const sputter::containers::FixedMemoryVector<sputter::math::FPVector2D>& vertexUVs
                );
            bool SetTextureCoordinates(
                const sputter::math::FPVector2D* uvArray, uint32_t arrayLen
                );

            bool SetIndices(
                const sputter::containers::FixedMemoryVector<int>& indices
                );
            bool SetIndices(
                const int* indicesArray, uint32_t arrayLen
                );

            bool SetShader(ShaderPtr spShader);
            void SetModelMatrix(const glm::mat4& modelMatrix);

            // TO be set for each draw call
            void SetMeshUniforms(const std::initializer_list<MeshUniformValue>& uniformValues);

            void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

            void SetVisibility(bool newVisibility);
            bool GetVisibility() const;

            // TODO: Some kind of local fixed-point transform
            // TODO: texture(s)?

        private:
            void CopyTo(Mesh& other) const;
            void BindAttributes();
            void UnbindAttributes();

            // Attributes
            Attribute<glm::vec3>           m_VertexPositionAttribute;
            Attribute<glm::vec3>           m_VertexNormalAttribute;
            Attribute<glm::vec2>           m_VertexTextureCoordinateAttribute;
            IndexBuffer                    m_Indices;

            // Data
            containers::ArrayVector<glm::vec3, meshconstants::kMaxVertices> m_VertexPositions;
            containers::ArrayVector<glm::vec3, meshconstants::kMaxVertices> m_VertexNormals;
            containers::ArrayVector<glm::vec2, meshconstants::kMaxVertices> m_VertexTextureCoordinates;
            containers::ArrayVector<uint32_t, meshconstants::kMaxIndices>   m_VertexIndices;

            static const size_t kMaxMeshUniforms = 8;
            containers::ArrayVector<MeshUniformValue, kMaxMeshUniforms> m_MeshUniformValues;

            glm::mat4              m_ModelMatrix;

            ShaderPtr              m_spShader;

            uint32_t               m_VAO;

            uint32_t               m_ModelUniformHandle;
            uint32_t               m_ViewUniformHandle;
            uint32_t               m_ProjUniformHandle;

            bool                   m_isDirty   = true;
            bool                   m_isVisible = true;
    };
}}