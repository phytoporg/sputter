#pragma once

enum class GameMode
{
    Invalid = 0,
    Local,  // Local play only, no online play
    Server, // P2P server
    Client, // P2P client
    Max
};