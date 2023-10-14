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
    { 
        .Type = MessageType::RestartReady,
        .MessageSize = sizeof(RestartReadyMessage) 
    };
};

constexpr size_t SizeMax(size_t a, size_t b)
{
    return a > b ? a : b;
}

constexpr size_t GetMaxMessageSize() 
{
    size_t maxSize = sizeof(MessageHeader);
    maxSize = SizeMax(maxSize, sizeof(HelloMessage));
    maxSize = SizeMax(maxSize, sizeof(StartGameMessage));
    maxSize = SizeMax(maxSize, sizeof(InputsMessage));
    maxSize = SizeMax(maxSize, sizeof(RestartReadyMessage));
    return maxSize;
}
