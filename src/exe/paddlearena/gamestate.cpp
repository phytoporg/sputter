#include "gamestate.h"

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
    if (!TheBall.Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }
    if (!Player1Paddle.Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }
    if (!Player2Paddle.Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }
    if (!Arena.Serialize(pBuffer, pBytesWrittenOut, maxBytes)) { return false; }

    WRITE(Player1Score, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(Player1Score);

    WRITE(Player2Score, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(Player2Score);

    WRITE(WinningPlayer, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(WinningPlayer);

    WRITE(CurrentState, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CurrentState);

    WRITE(CountdownTimerHandle, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CountdownTimerHandle);

    WRITE(CountdownTimeRemaining, pBuffer, *pBytesWrittenOut, maxBytes);
    *pBytesWrittenOut += sizeof(CountdownTimeRemaining);

    WRITE(Frame, pBuffer, *pBytesWrittenOut, maxBytes);
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