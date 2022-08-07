#include "gamestate.h"

GameState::GameState(
    sputter::assets::AssetStorageProvider* pStorageProvider,
    sputter::game::SubsystemProvider* pSubsystemProvider) 
    : MainShip(pStorageProvider, pSubsystemProvider)
{}
