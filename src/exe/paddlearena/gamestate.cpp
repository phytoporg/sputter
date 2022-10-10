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