#include "objectstorage.h"
#include "object.h"

#include <sputter/core/check.h>

using namespace sputter;
using namespace sputter::game;
using namespace sputter::game::objectstorage;

static const uint32_t kMaxNumObjects = 16;
static Object* s_pObjectStorageArray[kMaxNumObjects] = {};
static uint32_t s_objectCount = 0;

ObjectHandle sputter::game::objectstorage::RegisterObject(Object* pObject)
{
    RELEASE_CHECK(pObject != nullptr, "Cannot register a null object");
    RELEASE_CHECK(s_objectCount < kMaxNumObjects, "Registering too many objects");

    // If the object already exists, just return that
    for (uint32_t i = 0; i < s_objectCount; ++i)
    {
        if (s_pObjectStorageArray[i] == pObject)
        {
            return static_cast<ObjectHandle>(i);
        }
    }

    RELEASE_CHECK(s_pObjectStorageArray[s_objectCount] == nullptr, "Next object storage location is already occupied");

    ObjectHandle newObjectHandle = s_objectCount;
    s_pObjectStorageArray[s_objectCount++] = pObject;

    return newObjectHandle;
}

Object* sputter::game::objectstorage::FindObject(ObjectHandle handle)
{
    RELEASE_CHECK(s_objectCount <= objectstorage::kInvalidHandle, "Object count has mysteriously surpassed the max number of objects");
    if (handle == objectstorage::kInvalidHandle || handle > s_objectCount)
    {
        return nullptr;
    }

    return s_pObjectStorageArray[handle];
}