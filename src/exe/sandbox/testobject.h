#pragma once

#include <sputter/game/object.h>

// TODO: all this biz
class TestObject : game::Object
{
public:
    TestObject(SubsystemProvider* pProvider);

    virtual void Tick(float dt) override;
};
