#include "object.h"

namespace sputter { namespace game {
    Object::Object(SubsystemProvider* pProvider)
    {
        m_components.fill(nullptr);
    }
}}
