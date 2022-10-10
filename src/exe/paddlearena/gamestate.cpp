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

GameState::GameState(
    sputter::assets::AssetStorageProvider* pStorageProvider,
    sputter::game::SubsystemProvider* pSubsystemProvider) 
    : TheBall(pStorageProvider, pSubsystemProvider),
      Player1Paddle(0, pStorageProvider, pSubsystemProvider),
      Player2Paddle(1, pStorageProvider, pSubsystemProvider),
      Arena(pStorageProvider, pSubsystemProvider)
{}