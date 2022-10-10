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

GameState::GameState(sputter::assets::AssetStorageProvider* pStorageProvider) 
    : TheBall(pStorageProvider),
      Player1Paddle(0, pStorageProvider),
      Player2Paddle(1, pStorageProvider),
      Arena(pStorageProvider)
{}