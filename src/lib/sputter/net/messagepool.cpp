#include "messagepool.h"

#include <cstring>

#include <sputter/log/log.h>
#include <sputter/core/check.h>

using namespace sputter;
using namespace sputter::net;

MessagePool::MessagePool(size_t numPoolEntries)
    : m_numPoolEntries(numPoolEntries) 
{
    RELEASE_CHECK(m_numPoolEntries < kMaxMessagePoolSize, "Message pool too large");
    memset(&m_poolOccupancy, 0, sizeof(m_poolOccupancy));
}

MessageHeader* MessagePool::NewMessage()
{
    size_t entriesSeen = 0;
    for (size_t i = 0; i < m_numPoolEntries; ++i)
    {
        if (!m_poolOccupancy[i])
        {
            ++m_currentEntryCount;
            m_poolOccupancy[i] = true;
            return reinterpret_cast<MessageHeader*>(&m_messagePoolEntries[i]);
        }
    }

    return nullptr;
}

void MessagePool::FreeMessage(MessageHeader* pMessageHeader)
{
    if (!pMessageHeader) { return; }

    auto pPoolEntry = reinterpret_cast<MessagePoolEntry*>(pMessageHeader);
    const size_t Index = pPoolEntry - &m_messagePoolEntries[0];

    RELEASE_CHECK(Index < m_numPoolEntries, "Unexpected pool entry index");
    RELEASE_CHECK(m_poolOccupancy[Index], "Inconsistent occupancy state (Index)");

    m_poolOccupancy[Index] = false;
    memset(&m_messagePoolEntries[Index], 0, sizeof(MessagePoolEntry));
}
