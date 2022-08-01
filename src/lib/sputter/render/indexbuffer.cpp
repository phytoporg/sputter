#include "indexbuffer.h"
#include "render.h"

using namespace sputter::render;

IndexBuffer::IndexBuffer() 
{
    glGenBuffers(1, &m_handle);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_handle);    
}

void IndexBuffer::Set(const uint32_t* inputArray, uint32_t arrayLength) 
{
    m_count = arrayLength;    
    const size_t uintSize = sizeof(uint32_t);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, uintSize * arrayLength, inputArray, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Set(const std::vector<uint32_t>& data) 
{
    Set(data.data(), static_cast<uint32_t>(data.size()));
}

uint32_t IndexBuffer::Count() const 
{
    return m_count;    
}

uint32_t IndexBuffer::GetHandle() const 
{
    return m_handle;
}