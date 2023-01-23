#pragma once

#include <cstdint>

namespace sputter { namespace game {
    class Object;
}}

namespace sputter { namespace game { namespace objectstorage {
    using ObjectHandle = uint32_t;
    static const ObjectHandle kInvalidHandle = static_cast<uint32_t>(-1);

    ObjectHandle RegisterObject(Object* pObject);

    Object* FindObject(ObjectHandle handle);

    // NOTE: object storage isn't currently serializable. Allocate all objects and handles prior to game
    // start!
    // 
    // Therefore, there is no object "unregistration."
    // bool UnregisterObject(ObjectHandle objectHandle);
}}}