#include "object.h"

#include <sputter/assets/assetstorageprovider.h>

using namespace sputter::game;

Object::Object(
    sputter::assets::AssetStorageProvider* pAssetProvider,
    SubsystemProvider* pSubsystemProvider)
: m_pAssetStorageProvider(pAssetProvider),
  m_pSubsystemProvider(pSubsystemProvider)
{
    m_components.fill(nullptr);
}