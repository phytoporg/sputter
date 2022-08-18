#include "object.h"

#include <sputter/assets/assetstorageprovider.h>

using namespace sputter::game;

Object::Object(
    uint32_t objectType,
    sputter::assets::AssetStorageProvider* pAssetProvider,
    SubsystemProvider* pSubsystemProvider)
: m_objectType(objectType),
  m_pAssetStorageProvider(pAssetProvider),
  m_pSubsystemProvider(pSubsystemProvider)
{
    m_components.fill(nullptr);
}

uint32_t Object::GetType() const
{
    return m_objectType;
}