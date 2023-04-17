// Facilitates generating human-readable frame state logs for debugging checksum mismatches in
// rollback frame states.
//
// *Not* thread safe for an individual slot.

#pragma once

#include <cstdint>
#include <cstddef>

#include <sputter/core/check.h>
#include <sputter/core/serializable.h>

void ToString(bool boolValue, char* pBuffer);

namespace sputter { namespace log {
    class FrameStateLogger
    {
    public:
        static constexpr size_t kLogFrameBufferSize = 0x8000;
        static constexpr size_t kMaxFrameLoggerSlots = 8;

        // Arbitrary
        static constexpr size_t kMaxStructNameLength = 128;
        static constexpr size_t kMaxPropertyNameLength = 128;
        static constexpr size_t kMaxValueLength = 128;

        static void SetSlot(size_t slotIndex);
        static void BeginFrame();
        static void EndFrame(uint32_t checksum);

        static void BeginStruct(const char* pStructName);
        static void EndStruct();

        static void WriteProperty(const char* pPropertyName, bool value);
        static void WriteProperty(const char* pPropertyName, int value);
        static void WriteProperty(const char* pPropertyName, unsigned long value);
        static void WriteProperty(const char* pPropertyName, uint32_t value);
        static void WriteProperty(const char* pPropertyName, uint8_t value);
        static void WriteProperty(const char* pPropertyName, int16_t value);

        template<typename T>
        static void WriteProperty(const char* pPropertyName, T value);

        template<typename T>
        static void WriteArrayProperty(const char* pPropertyName, T* pValues, size_t numValues);

        // Write to disk
        static void LogFrameSlot(int slot, int frameNumber, const char* pIdentifier);

    private:
        static void WritePropertyName(const char* pPropertyName, bool withNewLine = false);
        static void WritePropertyValue(const char* pValueString);

        static size_t s_SlotIndex;

        static char s_LogBuffers[kMaxFrameLoggerSlots][kLogFrameBufferSize];
        static size_t s_LogBufferSizes[kMaxFrameLoggerSlots];
        static size_t s_LogBufferIndentations[kMaxFrameLoggerSlots];
    };

    template<typename T>
    void FrameStateLogger::WriteProperty(const char* pPropertyName, T value)
    {
        WritePropertyName(pPropertyName);

        char buffer[kMaxValueLength];
        ToString(value, buffer);
        WritePropertyValue(buffer);
    }

    template<typename T>
    void FrameStateLogger::WriteArrayProperty(
        const char *pPropertyName,
        T *pValues,
        size_t numValues)
    {
        const bool WithNewLine = true;
        WritePropertyName(pPropertyName, WithNewLine);

        s_LogBufferIndentations[s_SlotIndex]++;
        for (size_t i = 0; i < numValues; ++i)
        {
            char indexStringBuffer[32] = {};
            sprintf(indexStringBuffer, "%u: ", i);
            WritePropertyName(indexStringBuffer);

            char valueStringBuffer[kMaxValueLength];
            ToString(pValues[i], valueStringBuffer);
            WritePropertyValue(valueStringBuffer);
        }
        s_LogBufferIndentations[s_SlotIndex]--;
    }
}}

#define WRITE_OBJECT(object, pBuffer, pBytesWrittenOut, maxBytes) \
    sputter::log::FrameStateLogger::BeginStruct(#object); \
    RELEASE_CHECK(object.Serialize(pBuffer, pBytesWrittenOut, maxBytes), "Failed to write object"); \
    sputter::log::FrameStateLogger::EndStruct();

#define WRITE_PROPERTY(toWrite, pDestination, offset, maxBytes) \
    sputter::log::FrameStateLogger::WriteProperty(#toWrite, (toWrite)); \
    WRITE(toWrite, pDestination, offset, maxBytes)

#define WRITE_ARRAY_PROPERTY(toWrite, numElements, pDestination, offset, maxBytes) \
    sputter::log::FrameStateLogger::WriteArrayProperty(     \
        #toWrite,                                           \
        (toWrite),                                          \
        numElements);                                       \
    WRITE_ARRAY(toWrite, numElements, pDestination, offset, maxBytes)
