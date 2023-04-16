#include "framestatelogger.h"
#include "log.h"

#include <sputter/core/check.h>

#include <filesystem>
#include <fstream>
#include <cstring>

using namespace sputter;
using namespace sputter::log;

#if defined(ENABLE_LOG_FRAMES)
size_t FrameStateLogger::s_SlotIndex = 0;
char FrameStateLogger::s_LogBuffers[FrameStateLogger::kMaxFrameLoggerSlots][FrameStateLogger::kLogFrameBufferSize];
size_t FrameStateLogger::s_LogBufferSizes[FrameStateLogger::kMaxFrameLoggerSlots];
size_t FrameStateLogger::s_LogBufferIndentations[FrameStateLogger::kMaxFrameLoggerSlots];

void FrameStateLogger::SetSlot(size_t slotIndex)
{
    RELEASE_CHECK(slotIndex < FrameStateLogger::kMaxFrameLoggerSlots, "slotIndex is too large");
    s_SlotIndex = slotIndex;
}

void FrameStateLogger::BeginFrame()
{
    s_LogBufferSizes[s_SlotIndex] = 0;
    s_LogBufferIndentations[s_SlotIndex] = 0;
}

void FrameStateLogger::EndFrame(uint32_t checksum)
{
    WriteProperty("Checksum", checksum);
}

void FrameStateLogger::BeginStruct(const char* pStructName)
{
    const char* pLast = &s_LogBuffers[s_SlotIndex][kLogFrameBufferSize];
    const size_t SlotPosition = s_LogBufferSizes[s_SlotIndex];
    char* pStart = &s_LogBuffers[s_SlotIndex][SlotPosition];
    char* pNext = pStart;
    const size_t IndentationLevel = s_LogBufferIndentations[s_SlotIndex];
    RELEASE_CHECK((pNext + IndentationLevel) < pLast, "Ran out of log buffer space");
    for (size_t i = 0; i < IndentationLevel; ++i)
    {
        *pNext = '\t';
        ++pNext;
    }

    const size_t StructNameLen = strnlen(pStructName, kMaxStructNameLength);
    RELEASE_CHECK(StructNameLen < kMaxStructNameLength, "Struct name too long");

    RELEASE_CHECK((pNext + StructNameLen) < pLast, "Ran out of log buffer space");
    strncpy(pNext, pStructName, StructNameLen);
    pNext += StructNameLen;

    RELEASE_CHECK((pNext + 2) < pLast, "Ran out of log buffer space");
    pNext[0] = ':';
    pNext[1] = '\n';
    pNext += 2;

    s_LogBufferSizes[s_SlotIndex] += (pNext - pStart);
    s_LogBufferIndentations[s_SlotIndex]++;
}

void FrameStateLogger::EndStruct()
{
    s_LogBufferIndentations[s_SlotIndex]--;
}

void FrameStateLogger::WriteProperty(const char* pPropertyName, bool value)
{
    WritePropertyName(pPropertyName);

    char ValueBuffer[kMaxValueLength] = {};
    sprintf(ValueBuffer, "%s", (value ? "true" : "false"));
    WritePropertyValue(ValueBuffer);
}

void FrameStateLogger::WriteProperty(const char* pPropertyName, int value)
{
    WritePropertyName(pPropertyName);

    char ValueBuffer[kMaxValueLength] = {};
    sprintf(ValueBuffer, "%d", value);
    WritePropertyValue(ValueBuffer);
}

void FrameStateLogger::WriteProperty(const char* pPropertyName, unsigned long value)
{
    WritePropertyName(pPropertyName);

    char ValueBuffer[kMaxValueLength] = {};
    sprintf(ValueBuffer, "%ul", value);
    WritePropertyValue(ValueBuffer);
}

void FrameStateLogger::WriteProperty(const char* pPropertyName, uint32_t value)
{
    WritePropertyName(pPropertyName);

    char ValueBuffer[kMaxValueLength] = {};
    sprintf(ValueBuffer, "%u", value);
    WritePropertyValue(ValueBuffer);
}

void FrameStateLogger::WriteProperty(const char *pPropertyName, uint8_t value)
{
    WritePropertyName(pPropertyName);

    char ValueBuffer[kMaxValueLength] = {};
    sprintf(ValueBuffer, "%hhu", value);
    WritePropertyValue(ValueBuffer);
}

void FrameStateLogger::WriteProperty(const char *pPropertyName, int16_t value)
{
    WritePropertyName(pPropertyName);

    char ValueBuffer[kMaxValueLength] = {};
    sprintf(ValueBuffer, "%hd", value);
    WritePropertyValue(ValueBuffer);
}

void FrameStateLogger::WritePropertyName(const char *pPropertyName)
{
    const char* pLast = &s_LogBuffers[s_SlotIndex][kLogFrameBufferSize];
    const size_t SlotPosition = s_LogBufferSizes[s_SlotIndex];
    char* pStart = &s_LogBuffers[s_SlotIndex][SlotPosition];
    char* pNext = pStart;
    const size_t IndentationLevel = s_LogBufferIndentations[s_SlotIndex];
    RELEASE_CHECK((pNext + IndentationLevel) < pLast, "Ran out of log buffer space");
    for (size_t i = 0; i < IndentationLevel; ++i)
    {
        *pNext = '\t';
        ++pNext;
    }

    const size_t PropertyNameLen = strnlen(pPropertyName, kMaxPropertyNameLength);
    RELEASE_CHECK(PropertyNameLen < kMaxPropertyNameLength, "Property name too long");

    RELEASE_CHECK((pNext + PropertyNameLen) < pLast, "Ran out of log buffer space");
    strncpy(pNext, pPropertyName, PropertyNameLen);
    pNext += PropertyNameLen;

    RELEASE_CHECK((pNext + 2) < pLast, "Ran out of log buffer space");
    pNext[0] = ':';
    pNext[1] = ' ';
    pNext += 2;

    s_LogBufferSizes[s_SlotIndex] += (pNext - pStart);
}

void FrameStateLogger::WritePropertyValue(const char *pValueString)
{
    const char* pLast = &s_LogBuffers[s_SlotIndex][kLogFrameBufferSize];
    const size_t SlotPosition = s_LogBufferSizes[s_SlotIndex];
    char* pStart = &s_LogBuffers[s_SlotIndex][SlotPosition];
    char* pNext = pStart;

    const size_t PropertyValueLen = strnlen(pValueString, kMaxPropertyNameLength);
    RELEASE_CHECK(PropertyValueLen < kMaxPropertyNameLength, "Property value too long");

    strncpy(pNext, pValueString, PropertyValueLen);
    pNext += PropertyValueLen;
    pNext[0] = '\n';
    s_LogBufferSizes[s_SlotIndex] += PropertyValueLen + 1;
}

void FrameStateLogger::LogFrameSlot(int slot, int frameNumber, const char *pIdentifier)
{
    // Always save under temp dir
    using namespace std;
    using namespace std::filesystem;

    char filenameBuffer[256];
    sprintf(filenameBuffer, "frame%d_%s.txt", frameNumber, pIdentifier);

    path targetPath = temp_directory_path() / filenameBuffer;
    ofstream out(targetPath.string());
    if (!out)
    {
        RELEASE_LOGLINE_ERROR(
            LOG_FRAMESTATE,
            "Failed to save frame log to file: %s",
            targetPath.c_str());
        return;
    }

    out.write(s_LogBuffers[slot], s_LogBufferSizes[slot]);
    out.flush();
}

#else
void FrameStateLogger::SetSlot(size_t slotIndex) {}
void FrameStateLogger::BeginFrame() {}
void FrameStateLogger::EndFrame(uint32_t checksum) {}
void FrameStateLogger::BeginStruct(const char* pStructName) {}
void FrameStateLogger::EndStruct() {}
void FrameStateLogger::WriteProperty(const char* pPropertyName, bool value) {}
void FrameStateLogger::WriteProperty(const char* pPropertyName, int value) {}
void FrameStateLogger::WriteProperty(const char* pPropertyName, unsigned long value) {}
void FrameStateLogger::WriteProperty(const char* pPropertyName, uint32_t value) {}
void FrameStateLogger::WriteProperty(const char* pPropertyName, uint8_t value) {}
void FrameStateLogger::WriteProperty(const char* pPropertyName, int16_t value) {}
void FrameStateLogger::WritePropertyName(const char *pPropertyName) {}
void FrameStateLogger::WritePropertyValue(const char *pValueString) {}
void FrameStateLogger::LogFrameSlot(int slot, int frameNumber, const char *pIdentifier) {}
#endif
