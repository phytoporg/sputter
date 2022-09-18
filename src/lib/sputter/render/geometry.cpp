#include "geometry.h"

#include <sputter/math/fpconstants.h>

#include <cstdint>
#include <cstring>

using namespace sputter::math;

bool sputter::render::geometry::MakeFixedUnitCube(
    FPVector3D* pVertexPositions, uint32_t numVertexPositions,
    FPVector3D* pVertexNormals,   uint32_t numVertexNormals,
    FPVector2D* pVertexUVs,       uint32_t numVertexUVs,
    int* pIndices,                uint32_t numIndices)
{
    // Cube's origin is at the center.

    static const uint32_t kExpectedNumVertexPositions = 8;
    static const uint32_t kExpectedNumIndices = 36;

    if (numVertexPositions != kExpectedNumVertexPositions ||
        numVertexNormals   != kExpectedNumVertexPositions ||
        numVertexUVs       != kExpectedNumVertexPositions ||
        numIndices         != kExpectedNumIndices)
    {
        return false;
    }

    const FixedPoint HalfCubeSize = FPOne / FPTwo;

    // Bottom face
    pVertexPositions[0] = FPVector3D(-HalfCubeSize, -HalfCubeSize, -HalfCubeSize);
    pVertexPositions[1] = FPVector3D( HalfCubeSize, -HalfCubeSize, -HalfCubeSize);
    pVertexPositions[2] = FPVector3D( HalfCubeSize, -HalfCubeSize,  HalfCubeSize);
    pVertexPositions[3] = FPVector3D(-HalfCubeSize, -HalfCubeSize,  HalfCubeSize);

    // Top face
    pVertexPositions[4] = FPVector3D(-HalfCubeSize,  HalfCubeSize, -HalfCubeSize);
    pVertexPositions[5] = FPVector3D( HalfCubeSize,  HalfCubeSize, -HalfCubeSize);
    pVertexPositions[6] = FPVector3D( HalfCubeSize,  HalfCubeSize,  HalfCubeSize);
    pVertexPositions[7] = FPVector3D(-HalfCubeSize,  HalfCubeSize,  HalfCubeSize);

    // Bottom face
    pVertexNormals[0] = FPVector3D(-HalfCubeSize, -HalfCubeSize, -HalfCubeSize).Normalized();
    pVertexNormals[1] = FPVector3D( HalfCubeSize, -HalfCubeSize, -HalfCubeSize).Normalized();
    pVertexNormals[2] = FPVector3D( HalfCubeSize, -HalfCubeSize,  HalfCubeSize).Normalized();
    pVertexNormals[3] = FPVector3D(-HalfCubeSize, -HalfCubeSize,  HalfCubeSize).Normalized(); 

    // Top face
    pVertexNormals[4] = FPVector3D(-HalfCubeSize, HalfCubeSize, -HalfCubeSize).Normalized();
    pVertexNormals[5] = FPVector3D( HalfCubeSize, HalfCubeSize, -HalfCubeSize).Normalized();
    pVertexNormals[6] = FPVector3D( HalfCubeSize, HalfCubeSize,  HalfCubeSize).Normalized();
    pVertexNormals[7] = FPVector3D(-HalfCubeSize, HalfCubeSize,  HalfCubeSize).Normalized(); 

    // Bottom face
    pVertexUVs[0] = FPVector2D(FPZero, FPZero);
    pVertexUVs[1] = FPVector2D(FPZero, FPZero);
    pVertexUVs[2] = FPVector2D(FPZero, FPZero);
    pVertexUVs[3] = FPVector2D(FPZero, FPZero);

    // Top face
    pVertexUVs[4] = FPVector2D(FPZero, FPZero);
    pVertexUVs[5] = FPVector2D(FPZero, FPZero);
    pVertexUVs[6] = FPVector2D(FPZero, FPZero);
    pVertexUVs[7] = FPVector2D(FPZero, FPZero);

    // Bottom face:
    // 3 ***** 2  |
    // *       *  | Front
    // *       *  |
    // *       *  V
    // 0 ***** 1
    //
    // Top face:
    // 7 ***** 6
    // *       *
    // *       *
    // *       *
    // 4 ***** 5

    // Assuming GL_CCW for front-facing
    const int IndexValues[] = {
        // Bottom face
        2, 1, 0, 2, 0, 3,
        // Front face
        5, 0, 1, 4, 0, 5,
        // Left face
        4, 3, 0, 7, 3, 4,
        // Right face
        5, 1, 2, 6, 2, 5,
        // Back face
        6, 2, 3, 7, 6, 3,
        // Top face
        6, 5, 4, 7, 6, 4,
    };
    memcpy(pIndices, IndexValues, sizeof(IndexValues));

    return true;
}

bool sputter::render::geometry::MakeUnitCube(
    glm::vec3* pVertexPositions, uint32_t numVertexPositions,
    glm::vec3* pVertexNormals,   uint32_t numVertexNormals,
    glm::vec2* pVertexUVs,       uint32_t numVertexUVs,
    int* pIndices,               uint32_t numIndices)
{
    // Cube's origin is at the center.
    static const uint32_t kExpectedNumVertexPositions = 8;
    static const uint32_t kExpectedNumIndices = 36;

    if (numVertexPositions != kExpectedNumVertexPositions ||
        numVertexNormals   != kExpectedNumVertexPositions ||
        numVertexUVs       != kExpectedNumVertexPositions ||
        numIndices         != kExpectedNumIndices)
    {
        return false;
    }

    const FixedPoint HalfCubeSize = FPOne / FPTwo;

    // Bottom face
    pVertexPositions[0] = glm::vec3(-HalfCubeSize, -HalfCubeSize, -HalfCubeSize);
    pVertexPositions[1] = glm::vec3( HalfCubeSize, -HalfCubeSize, -HalfCubeSize);
    pVertexPositions[2] = glm::vec3( HalfCubeSize, -HalfCubeSize,  HalfCubeSize);
    pVertexPositions[3] = glm::vec3(-HalfCubeSize, -HalfCubeSize,  HalfCubeSize);

    // Top face
    pVertexPositions[4] = glm::vec3(-HalfCubeSize,  HalfCubeSize, -HalfCubeSize);
    pVertexPositions[5] = glm::vec3( HalfCubeSize,  HalfCubeSize, -HalfCubeSize);
    pVertexPositions[6] = glm::vec3( HalfCubeSize,  HalfCubeSize,  HalfCubeSize);
    pVertexPositions[7] = glm::vec3(-HalfCubeSize,  HalfCubeSize,  HalfCubeSize);

    // Bottom face
    pVertexNormals[0] = glm::normalize(glm::vec3(-HalfCubeSize, -HalfCubeSize, -HalfCubeSize));
    pVertexNormals[1] = glm::normalize(glm::vec3( HalfCubeSize, -HalfCubeSize, -HalfCubeSize));
    pVertexNormals[2] = glm::normalize(glm::vec3( HalfCubeSize, -HalfCubeSize,  HalfCubeSize));
    pVertexNormals[3] = glm::normalize(glm::vec3(-HalfCubeSize, -HalfCubeSize,  HalfCubeSize));

    // Top face
    pVertexNormals[4] = glm::normalize(glm::vec3(-HalfCubeSize, HalfCubeSize, -HalfCubeSize));
    pVertexNormals[5] = glm::normalize(glm::vec3( HalfCubeSize, HalfCubeSize, -HalfCubeSize));
    pVertexNormals[6] = glm::normalize(glm::vec3( HalfCubeSize, HalfCubeSize,  HalfCubeSize));
    pVertexNormals[7] = glm::normalize(glm::vec3(-HalfCubeSize, HalfCubeSize,  HalfCubeSize));

    // Bottom face
    pVertexUVs[0] = glm::vec2(FPZero, FPZero);
    pVertexUVs[1] = glm::vec2(FPZero, FPZero);
    pVertexUVs[2] = glm::vec2(FPZero, FPZero);
    pVertexUVs[3] = glm::vec2(FPZero, FPZero);

    // Top face
    pVertexUVs[4] = glm::vec2(FPZero, FPZero);
    pVertexUVs[5] = glm::vec2(FPZero, FPZero);
    pVertexUVs[6] = glm::vec2(FPZero, FPZero);
    pVertexUVs[7] = glm::vec2(FPZero, FPZero);

    // Bottom face:
    // 3 ***** 2  |
    // *       *  | Front
    // *       *  |
    // *       *  V
    // 0 ***** 1
    //
    // Top face:
    // 7 ***** 6
    // *       *
    // *       *
    // *       *
    // 4 ***** 5

    // Assuming GL_CCW for front-facing
    const int IndexValues[] = {
        // Bottom face
        2, 1, 0, 2, 0, 3,
        // Front face
        5, 0, 1, 4, 0, 5,
        // Left face
        4, 3, 0, 7, 3, 4,
        // Right face
        5, 1, 2, 6, 2, 5,
        // Back face
        6, 2, 3, 7, 6, 3,
        // Top face
        6, 5, 4, 7, 6, 4,
    };
    memcpy(pIndices, IndexValues, sizeof(IndexValues));

    return true;
}

bool sputter::render::geometry::MakeBorderedRect(
    int32_t x, int32_t y, int32_t width, int32_t height, int32_t borderSize,
    glm::ivec2* pVertexPositions, uint32_t numVertexPositions,
    int* pIndices, uint32_t numIndices)
{
    static const uint32_t kExpectedNumVertexPositions = 8;
    static const uint32_t kExpectedNumIndices = 24;

    if (numVertexPositions != kExpectedNumVertexPositions ||
        numIndices         != kExpectedNumIndices)
    {
        return false;
    }

    // Inner & outer vertex indices (not to scale, this is low-effort illustrative ascii art):
    //
    // 4---------------5
    // |               |
    // |  0---------1  |
    // |  |         |  |
    // |  3---------2  |
    // |               |
    // 7---------------6

    // Inner vertices
    pVertexPositions[0] = glm::ivec2(x, y);
    pVertexPositions[1] = glm::ivec2(x + width, y);
    pVertexPositions[2] = glm::ivec2(x + width, y + height);
    pVertexPositions[3] = glm::ivec2(x, y + height);

    // Outer vertices
    const int32_t OuterX = x - borderSize;
    const int32_t OuterY = y - borderSize;
    const int32_t OuterWidth  = width + 2 * borderSize;
    const int32_t OuterHeight = height + 2 * borderSize;

    pVertexPositions[4] = glm::ivec2(OuterX, OuterY);
    pVertexPositions[5] = glm::ivec2(OuterX + OuterWidth, OuterY);
    pVertexPositions[6] = glm::ivec2(OuterX + OuterWidth, OuterY + OuterHeight);
    pVertexPositions[7] = glm::ivec2(OuterX, OuterY + OuterHeight);

    // Indices
    // Assuming GL_CCW for front-facing
    const int IndexValues[] = {
        // Top
        0, 5, 4, 1, 5, 0,
        // Right
        5, 1, 6, 1, 2, 6,
        // Bottom
        2, 3, 6, 3, 7, 6,
        // Left
        3, 0, 7, 0, 4, 7,
    };
    memcpy(pIndices, IndexValues, sizeof(IndexValues));

    return true;
}