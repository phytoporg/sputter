#include "gamestate.h"

GameState::GameState(
    sputter::assets::AssetStorageProvider* pStorageProvider,
    sputter::game::SubsystemProvider* pSubsystemProvider) 
    : Player1Paddle(0, pStorageProvider, pSubsystemProvider),
      Player2Paddle(1, pStorageProvider, pSubsystemProvider),
      Arena(pStorageProvider, pSubsystemProvider)
{}
