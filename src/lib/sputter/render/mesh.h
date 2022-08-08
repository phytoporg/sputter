#pragma once

#include "shader.h"
#include <sputter/math/fpvector3d.h>
#include <sputter/containers/fixedmemoryvector.h>

namespace sputter { namespace render {
    class Shader;

    class Mesh 
    {
        public:
            Mesh(uint32_t maxVertices);

            bool SetVertices(
                const sputter::containers::FixedMemoryVector<sputter::math::FPVector3D>& vertices
                );
            bool SetIndices(
                const sputter::containers::FixedMemoryVector<int>& indices
                );
            bool SetShader(ShaderPtr spShader);

            // TODO: Some kind of local fixed-point transform

        private:
            Mesh() = delete;
            Mesh(const Mesh& other) = delete;
            Mesh& operator=(const Mesh& other) = delete;

            // TODO: not this?
            sputter::containers::FixedMemoryVector<sputter::math::FPVector3D> m_vertices;
            sputter::containers::FixedMemoryVector<int>                       m_indices;

            uint32_t                                                          m_maxVertices;

            ShaderPtr                                                         m_spShader;
    };
}}