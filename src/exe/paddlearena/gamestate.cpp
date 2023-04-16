#include "gamestate.h"
#include <sputter/log/framestatelogger.h>
#include <cstring>

GameState* GameState::s_pGameState = nullptr;

GameState* GameState::GetGameStateAddress()
{
    return s_pGameState;
}

void GameState::SetGameStateAddress(GameState* pGameState)
{
    s_pGameState = pGameState;
}

GameState::GameState() : Player1Paddle(0), Player2Paddle(1)
{}

bool GameState::Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes)
{
    // Don't need to serialize the camera... yet?
    WRITE_OBJECT(TheBall, pBuffer, pBytesWrittenOut, maxBytes);
    WRITE_OBJECT(Player1Paddle, pBuffer, pBytesWrittenOut, maxBytes);
    WRITE_OBJECT(Player2Paddle, pBuffer, pBytesWrittenOut, maxBytes);
    WRITE_OBJECT(Arena, pBuffer, pBytesWrittenOut, maxBytes);

    WRITE_PROPERTY(Player1Score, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(Player1Score);

    WRITE_PROPERTY(Player2Score, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(Player2Score);

    WRITE_PROPERTY(WinningPlayer, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(WinningPlayer);

    WRITE_PROPERTY(CurrentState, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CurrentState);

    WRITE_PROPERTY(CountdownTimerHandle, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CountdownTimerHandle);

    WRITE_PROPERTY(CountdownTimeRemaining, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CountdownTimeRemaining);

    WRITE_PROPERTY(Frame, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(Frame);

    return true;
}

bool GameState::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    // Don't need to serialize the camera... yet?
    if (!TheBall.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!Player1Paddle.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!Player2Paddle.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!Arena.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }

    READ(Player1Score, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(Player1Score);

    READ(Player2Score, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(Player2Score);

    READ(WinningPlayer, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(WinningPlayer);

    READ(CurrentState, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CurrentState);

    READ(CountdownTimerHandle, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CountdownTimerHandle);

    READ(CountdownTimeRemaining, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(CountdownTimeRemaining);

    READ(Frame, pBuffer, *pBytesReadOut, maxBytes);
    *pBytesReadOut += sizeof(Frame);

    return true;
}

void ToString(GameState::State state, char *pBuffer)
{
    RELEASE_CHECK(
        state < GameState::State::MaxValue && state > GameState::State::Invalid,
        "Unexpected state value");
    static const char* pStateStrings[] = {
        "Invalid",
        "Starting",
        "Playing",
        "Paused",
        "Ended",
        "Exiting",
        "Restarting",
    };

    static constexpr size_t NumStringEntries = sizeof(pStateStrings) / sizeof(pStateStrings[0]);
    static_assert(
        static_cast<int>(GameState::State::MaxValue) == NumStringEntries,
        "State string amount does not match number of enum entries");
    const int StateInt = static_cast<int>(state);
    strncpy(pBuffer, pStateStrings[StateInt], sizeof(pStateStrings[StateInt]));
}
