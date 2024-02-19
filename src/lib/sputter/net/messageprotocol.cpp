#include "messageprotocol.h"
#include <cstring>

namespace {
    template <typename T>
    T* FromHeaderHelper(MessageHeader* pHeader)
    {
        if (pHeader->Type != T::Type)
        {
            return nullptr;
        }

        if (pHeader->MessageSize != T::GetExpectedSize())
        {
            return nullptr;
        }

        return reinterpret_cast<T*>(pHeader);
    }
}

//
// HelloMessage
//
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

//
// AssignClientId
//
size_t AssignClientIdMessage::GetExpectedSize()
{
    return sizeof(AssignClientIdMessage);
}

bool CreateAssignClientIdMessage(uint8_t clientId, AssignClientIdMessage& messageOut)
{
    messageOut.Header.Type = MessageType::AssignClientId;
    messageOut.Header.MessageSize = AssignClientIdMessage::GetExpectedSize();

    messageOut.ClientId = clientId;
    return true;
}

AssignClientIdMessage* AssignClientIdFromHeader(MessageHeader* pHeader)
{
    return FromHeaderHelper<AssignClientIdMessage>(pHeader);
}

//
// ClientReady
//
size_t ClientReadyMessage::GetExpectedSize()
{
    return sizeof(ClientReadyMessage);
}

bool CreateClientReadyMessage(uint8_t clientId, ClientReadyMessage& messageOut)
{
    messageOut.Header.Type = MessageType::ClientReady;
    messageOut.Header.MessageSize = ClientReadyMessage::GetExpectedSize();

    messageOut.ClientId = clientId;
    return true;
}

//
// StartGame
//

size_t StartGameMessage::GetExpectedSize()
{
    return sizeof(StartGameMessage);
}

bool CreateStartGameMessage(uint32_t gameID, StartGameMessage& messageOut)
{
    messageOut.Header.Type = MessageType::StartGame;
    messageOut.Header.MessageSize = StartGameMessage::GetExpectedSize();

    messageOut.GameID = gameID;
    return true;
}
StartGameMessage* StartGameFromHeader(MessageHeader* pHeader)
{
    return FromHeaderHelper<StartGameMessage>(pHeader);
}

//
// InputsMessage
//
size_t InputsMessage::GetExpectedSize(size_t numInputMasks)
{
    return sizeof(InputsMessage) + numInputMasks * sizeof(InputsMessage::GameInputMasks[0]);
}
