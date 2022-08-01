#include "draw.h"
#include "indexbuffer.h"
#include "render.h"
#include <cassert>

using namespace sputter::render;

namespace 
{
    GLenum DrawModeToGlEnum(DrawMode drawMode)
    {
        static const GLenum lookupTable[] = {
            static_cast<GLenum>(-1), // DrawMode::Invalid
            GL_POINTS,               // DrawMode::Points
            GL_LINE_STRIP,           // DrawMode::LineStrip
            GL_LINE_LOOP,            // DrawMode::LineLoop
            GL_LINES,                // DrawMode::Lines
            GL_TRIANGLES,            // DrawMode::Triangles
            GL_TRIANGLE_STRIP,       // DrawMode::TriangleStrip
            GL_TRIANGLE_FAN,         // DrawMode::TriangleFan
            static_cast<GLenum>(-1), // DrawMode::MaxValue
        };

        if (drawMode > DrawMode::Invalid && drawMode < DrawMode::MaxValue)
        {
            return lookupTable[static_cast<uint32_t>(drawMode)];
        }

        // Shouldn't get here
        assert(false);
        return lookupTable[0]; // Invalid !
    }

}

void Draw(const IndexBuffer& inBuffer, DrawMode mode)
{
    const uint32_t handle = inBuffer.GetHandle();
    const uint32_t numIndices = inBuffer.Count();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glDrawElements(DrawModeToGlEnum(mode), numIndices, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Draw(uint32_t vertexCount, DrawMode mode) 
{
    glDrawArrays(DrawModeToGlEnum(mode), 0, vertexCount);
}

void DrawInstanced(const IndexBuffer& inBuffer, DrawMode mode, uint32_t instanceCount)
{
    const uint32_t handle = inBuffer.GetHandle();
    const uint32_t numIndices = inBuffer.Count();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glDrawElementsInstanced(DrawModeToGlEnum(mode), numIndices, GL_UNSIGNED_INT, 0, instanceCount);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawInstanced(uint32_t vertexCount, DrawMode mode, uint32_t instanceCount) 
{
    glDrawArraysInstanced(DrawModeToGlEnum(mode), 0, vertexCount, instanceCount);
}
