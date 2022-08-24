#pragma once

#include <sputter/math/fpvector3d.h>
#include <sputter/math/fpvector2d.h>

namespace sputter { namespace render { namespace geometry {
    bool MakeUnitCube(
        sputter::math::FPVector3D* pVertexPositions, uint32_t numVertexPositions,
        sputter::math::FPVector3D* pVertexNormals,   uint32_t numVertexNormals,
        sputter::math::FPVector2D* pVertexUVs,       uint32_t numVertexUVs,
        int* pIndices,                               uint32_t numIndices);
}}}