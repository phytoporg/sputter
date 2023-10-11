#pragma once

#include <cstddef>
#include <cstdint>

enum class MessageType
{
    Invalid = 0,
    Hello,
    StartGame,
    Inputs,
    RestartReady,
    Goodbye,
    Max
};

struct MessageHeader
{
    MessageType Type = MessageType::Invalid;
    size_t MessageSize = 0;
};

struct HelloMessage
{
    static size_t GetExpectedSize(const char* pName, uint8_t nameSize);

    MessageHeader Header;
    uint8_t NameSize;
    char Name[256];
};
bool CreateHelloMessage(const char* pName, uint8_t nameSize, HelloMessage& messageOut);

struct StartGameMessage
{
    static size_t GetExpectedSize(size_t numInputMasks);

    MessageHeader Header;
    uint32_t GameID;
};

struct InputsMessage
{
    static size_t GetExpectedSize(size_t numInputMasks);

    MessageHeader Header;
    uint8_t NumFrames;
    int32_t StartFrame;
    uint32_t GameInputMasks[];
};

struct RestartReadyMessage
{
    MessageHeader Header =
        { .Type = MessageType::RestartReady, .MessageSize = sizeof(RestartReadyMessage) };
};

// TODO: checksum?
