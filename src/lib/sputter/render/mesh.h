#pragma once

#include "shader.h"
#include <memory>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>
#include <sputter/containers/fixedmemoryvector.h>
#include <sputter/game/subsystemtype.h>

namespace sputter { namespace render {
    class Shader;

    class Mesh 
    {
        public:
            static const sputter::game::SubsystemType ComponentId = 
                sputter::game::SubsystemType::TYPE_MESH;

            struct InitializationParameters {};

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

            void Draw(const glm::mat4& projMatrix, const glm::mat4& viewMatrix);

            // TODO: Some kind of local fixed-point transform
            // TODO: texture(s)?

        private:
            struct PImpl;
            std::shared_ptr<PImpl> m_spPimpl = nullptr;
    };
}}