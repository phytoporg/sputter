#pragma once

#include "shader.h"
#include <string>
#include <vector>
#include <cstdint>

#include <sputter/assets/resourcestorage.h>
#include <sputter/assets/resourcestoragetype.h>

// Forward declarations
namespace sputter { namespace assets {
    struct TextData;
    class AssetStorage;
}}

namespace sputter { namespace render {
    class ShaderStorage : public sputter::assets::IResourceStorage<Shader>
    {
    public:
        static const assets::ResourceStorageType StorageTypeId =
            assets::ResourceStorageType::TYPE_SHADER;

        ~ShaderStorage();

        bool 
        AddShaderFromShaderAssetNames(
            sputter::assets::AssetStorage* pAssetStorage, 
            const std::string& vertexShaderAssetName,
            const std::string& fragmentShaderAssetName,
            const std::string& shaderName
            );

        bool
        AddShader(
            const assets::TextData& vertexText,
            const assets::TextData& fragmentText,
            const std::string& shaderName
            );

        ShaderPtr FindShaderByName(const std::string& shaderName) const;

    private:
        uint32_t CompileVertexShader(const std::string& source);
        uint32_t CompileFragmentShader(const std::string& source);

        // IResourceStorage
        virtual bool AddResource(Shader* pTexture) override;
        virtual bool ReleaseResource(Shader* pTexture) override;

        // TODO:
        // LinkShaders
        bool 
        LinkShaders(
            uint32_t programHandle,
            uint32_t vertexShaderHandle,
            uint32_t fragmentShaderHandle
            );
        std::vector<ShaderAttribute> PopulateAttributes(uint32_t programHandle);
        std::vector<ShaderUniform> PopulateUniforms(uint32_t programHandle);

        std::vector<ShaderPtr> m_storageVector;
    };
}}