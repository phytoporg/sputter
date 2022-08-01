#pragma once

#include <cstdint>

namespace sputter { namespace render {
    // Forward declarations
    class IndexBuffer;

    enum class DrawMode
    {
        Invalid = 0,
        Points,
        LineStrip,
        LineLoop,
        Lines,
        Triangles,
        TriangleStrip,
        TriangleFan,
        MaxValue,
    };

    void Draw(const IndexBuffer& inBuffer, DrawMode mode);
    void Draw(uint32_t vertexCount, DrawMode mode);

    void DrawInstanced(const IndexBuffer& inBuffer, DrawMode mode, uint32_t instanceCount);
    void DrawInstanced(uint32_t vertexCount, DrawMode mode, uint32_t instanceCount);
}}