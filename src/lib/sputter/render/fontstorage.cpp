#include "fontstorage.h"
#include "truetypeparser.h"

#include <sputter/assets/assetstorage.h>
#include <sputter/assets/binarydata.h>

#include <sputter/system/system.h>

using namespace sputter::render;

bool
FontStorage::AddFontFromAssetName(
    assets::AssetStorage* pAssetStorage,
    const std::string& fontAssetName
)
{
    std::shared_ptr<assets::AssetData> spAssetData = pAssetStorage->FindFirstByName(fontAssetName);
    auto* pBinaryData = static_cast<assets::BinaryData*>(spAssetData.get());

    return AddFont(*pBinaryData, fontAssetName);
}

bool
FontStorage::AddFont(
    const assets::BinaryData& binaryData,
    const std::string& fontName
)
{
    TrueTypeParser* pParser = new TrueTypeParser(binaryData);
    if (pParser && pParser->IsGood())
    {
        m_storageVector.emplace_back(std::make_shared<Font>(fontName, pParser));
        return true;
    }
    
    return false;
}

FontPtr FontStorage::FindFontByName(const std::string& fontName) const
{
    for (FontPtr spFont : m_storageVector)
    {
        if (spFont->GetName() == fontName)
        {
            return spFont;
        }
    }

    return nullptr;
}

bool FontStorage::AddResource(Font* pFont)
{
    // TODO: I have a todo item to kill this interface. These functions serve no useful
    // purpose.
    system::LogAndFail("Unimplemented function.");

    // Appease the compiler
    return false;
}

bool FontStorage::ReleaseResource(Font* pFont)
{
    // This function is dumb and we don't need it!
    system::LogAndFail("Unimplemented function.");

    // Appease the compiler
    return false;
}