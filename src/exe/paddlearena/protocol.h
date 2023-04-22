#pragma once

#include <cstddef>
#include <cstdint>

enum class MessageType
{
    Invalid = 0,
    Hello,
    Inputs,
    Max
};

struct MessageHeader
{
    MessageType Type = MessageType::Invalid;
    size_t MessageSize = 0;
};

struct HelloMessage
{
    MessageHeader Header = { .Type = MessageType::Hello, .MessageSize = sizeof(HelloMessage) };
};

struct InputsMessage
{
    static size_t GetExpectedSize(size_t numInputMasks);

    MessageHeader Header;
    uint8_t NumFrames;
    uint32_t StartFrame;
    uint32_t GameInputMasks[];
};


// TODO: checksum?
