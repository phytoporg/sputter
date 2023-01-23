#include "object.h"

#include <sputter/assets/assetstorageprovider.h>

using namespace sputter::game;

Object::Object(uint32_t objectType) : m_objectType(objectType)
{
    memset(m_components, 0, sizeof(m_components));
    m_handle = objectstorage::RegisterObject(this);
}

uint32_t Object::GetType() const
{
    return m_objectType;
}

objectstorage::ObjectHandle Object::GetHandle() const
{
    return m_handle;
}