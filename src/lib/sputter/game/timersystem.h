#pragma once

#include <cstdint>
#include <sputter/core/functor.h>
#include <sputter/core/functorstorage.h>
#include <sputter/core/serializable.h>

namespace sputter { namespace game {
    using TimerHandle = uint32_t;
    static const TimerHandle kInvalidTimerHandle = 0xFFFFFFFF;

    struct TimerEntry : public sputter::core::ISerializable
    {
        // ++ISerializable
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
        virtual const char * GetName() const override { return "TimerEntry"; };
        // --ISerializable

        TimerHandle Handle = kInvalidTimerHandle;
        core::Functor OnTimerExpired;
        uint64_t InitialFrames = 0;
        uint64_t FramesRemaining = 0;
        int8_t LoopCount = -1;
    };

    // Booo, I don't like this name but "TimerManager" is worse
    class TimerSystem : public core::ISerializable
    {
    public:

        // ++ISerializable
        virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
        virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
        virtual const char * GetName() const override { return "TimerSystem"; };
        // --ISerializable

        TimerHandle CreateFrameTimer(uint32_t numFrames, void (*pfnCallback)(void*) = nullptr, void* pUserData = nullptr);
        TimerHandle CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, void (*pfnCallback)(void*) = nullptr, void* pUserData = nullptr);

        TimerHandle CreateFrameTimer(uint32_t numFrames, const core::Functor& onTimerExpired = core::Functor());
        TimerHandle CreateLoopingFrameTimer(uint32_t numFrames, int8_t loopCount, const core::Functor& onTimerExpired = core::Functor());
        bool ClearTimer(TimerHandle timerHandle);

        void Tick();

    private:
        static const uint8_t kMaxTimerEntries = 8;

        TimerEntry  m_entries[kMaxTimerEntries];
        TimerHandle m_nextTimerHandle = 0;
        uint8_t     m_nextEntry = 0;
    };
}}

void ToString(const sputter::game::TimerEntry& timerEntry, char* pBuffer);