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

    size_t bytesWritten = 0;
    WRITE(Player1Score, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(Player1Score);

    WRITE(Player2Score, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(Player2Score);

    WRITE(WinningPlayer, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(WinningPlayer);

    WRITE(CurrentState, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(CurrentState);

    WRITE(CountdownTimerHandle, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(CountdownTimerHandle);

    WRITE(CountdownTimeRemaining, pBuffer, bytesWritten, maxBytes);
    bytesWritten += sizeof(CountdownTimeRemaining);

    *pBytesWrittenOut += bytesWritten;
    return true;
}

bool GameState::Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes)
{
    // Don't need to serialize the camera... yet?
    if (!TheBall.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!Player1Paddle.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!Player2Paddle.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }
    if (!Arena.Deserialize(pBuffer, pBytesReadOut, maxBytes)) { return false; }

    size_t bytesRead = 0;
    READ(Player1Score, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(Player1Score);

    READ(Player2Score, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(Player2Score);

    READ(WinningPlayer, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(WinningPlayer);

    READ(CurrentState, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(CurrentState);

    READ(CountdownTimerHandle, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(CountdownTimerHandle);

    READ(CountdownTimeRemaining, pBuffer, bytesRead, maxBytes);
    bytesRead += sizeof(CountdownTimeRemaining);

    *pBytesReadOut += bytesRead;
    return true;
}