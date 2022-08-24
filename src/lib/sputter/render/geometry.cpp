#include "geometry.h"

#include <sputter/math/fpconstants.h>

#include <cstdint>
#include <cstring>

using namespace sputter::math;

bool sputter::render::geometry::MakeUnitCube(
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
        numIndices         != kExpectedNumIndices
        )
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