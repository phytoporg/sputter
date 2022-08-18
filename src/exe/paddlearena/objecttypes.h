#pragma once

#include <cstdint>

enum class PaddleArenaObjectType
{
    Invalid = 0,
    Paddle,
    Stage,
    Ball,
    MaxPaddleArenaObjectType
};

// TODO: Find a nicer way :(
// uint32_t constants to avoid casts everywhere. 
#define DECLARE_UINT_FROM_ENUM(EnumClass, Value) \
    static const uint32_t k##EnumClass##Value = static_cast<uint32_t>(EnumClass::Value)

DECLARE_UINT_FROM_ENUM(PaddleArenaObjectType, Invalid);
DECLARE_UINT_FROM_ENUM(PaddleArenaObjectType, Paddle);
DECLARE_UINT_FROM_ENUM(PaddleArenaObjectType, Stage);
DECLARE_UINT_FROM_ENUM(PaddleArenaObjectType, Ball);

#undef DECLARE_UINT_FROM_ENUM
