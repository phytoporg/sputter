#include "gamestate.h"

GameState::GameState(
    sputter::assets::AssetStorageProvider* pStorageProvider,
    sputter::game::SubsystemProvider* pSubsystemProvider) 
    : TheBall(pStorageProvider, pSubsystemProvider),
      Player1Paddle(this, 0, pStorageProvider, pSubsystemProvider),
      Player2Paddle(this, 1, pStorageProvider, pSubsystemProvider),
      Arena(pStorageProvider, pSubsystemProvider)
{}
