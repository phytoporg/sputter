#pragma once

// A resource storage object in Sputter manages the loading and lifetime of a particular 
// kind of resource specific to a subsystem. Could be a texture, an audio sample-- basically
// an asset loaded and transformed into something that must be managed and circulated at 
// runtime.

namespace sputter { namespace assets {
    template<typename ResourceType>
    class IResourceStorage
    {
    public:
        // Returns true or false based on whether the resource could successfully be added.
        virtual bool AddResource(ResourceType* pResource) = 0;

        // Only returns false if pResource could not be found in storage. Release should never
        // fail otherwise.
        virtual bool ReleaseResource(ResourceType* pResource) = 0;
    };
}}
