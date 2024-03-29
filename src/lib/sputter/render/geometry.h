#pragma once

#include "color.h"

#include <sputter/math/fpvector3d.h>
#include <sputter/math/fpvector2d.h>

#include <glm/glm.hpp>

namespace sputter { namespace render { namespace geometry {
    bool MakeFixedUnitCube(
        sputter::math::FPVector3D* pVertexPositions, uint32_t numVertexPositions,
        sputter::math::FPVector3D* pVertexNormals,   uint32_t numVertexNormals,
        sputter::math::FPVector2D* pVertexUVs,       uint32_t numVertexUVs,
        int* pIndices,                               uint32_t numIndices);

    bool MakeUnitCube(
        glm::vec3* pVertexPositions, uint32_t numVertexPositions,
        glm::vec3* pVertexNormals,   uint32_t numVertexNormals,
        glm::vec2* pVertexUVs,       uint32_t numVertexUVs,
        int* pIndices,               uint32_t numIndices);

    // Not using fixed point input here since atm this is only used for UI rendering. Same reasoning
    // for lack of UVs, Normals.
    bool MakeLine(
        int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t z, int32_t size, 
        glm::vec3* pVectorPositions, uint32_t numVertexPositions, 
        int* pIndices, uint32_t numIndices);

    bool MakeBorderedRect(
        int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t borderSize,
        glm::vec3* pVertexPositions, uint32_t numVertexPositions,
        int* pIndices, uint32_t numIndices);

    bool MakeQuad(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, 
        glm::vec3* pVertexPositions, uint32_t numVertexPositions,
        int* pIndices, uint32_t numIndices);
}}}
