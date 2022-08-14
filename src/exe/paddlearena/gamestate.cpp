#include "gamestate.h"

GameState::GameState(
    sputter::assets::AssetStorageProvider* pStorageProvider,
    sputter::game::SubsystemProvider* pSubsystemProvider) 
    : Player1Paddle(pStorageProvider, pSubsystemProvider),
      Arena(pStorageProvider, pSubsystemProvider)
{}
