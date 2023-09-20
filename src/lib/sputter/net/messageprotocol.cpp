#include "protocol.h"

size_t InputsMessage::GetExpectedSize(size_t numInputMasks)
{
    return sizeof(InputsMessage) + numInputMasks * sizeof(InputsMessage::GameInputMasks[0]);
}
