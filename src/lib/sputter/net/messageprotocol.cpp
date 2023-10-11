#include "messageprotocol.h"
#include <cstring>

size_t HelloMessage::GetExpectedSize(const char* pName, uint8_t nameSize)
{
    return sizeof(MessageHeader) + nameSize + sizeof(uint8_t);
}

bool CreateHelloMessage(const char* pName, uint8_t nameSize, HelloMessage& messageOut)
{
    messageOut.Header.Type = MessageType::Hello;
    messageOut.Header.MessageSize = HelloMessage::GetExpectedSize(pName, nameSize);

    memcpy(messageOut.Name, pName, nameSize);
    return true;
}

size_t InputsMessage::GetExpectedSize(size_t numInputMasks)
{
    return sizeof(InputsMessage) + numInputMasks * sizeof(InputsMessage::GameInputMasks[0]);
}
