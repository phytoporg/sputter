#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/render/camera.h>
#include <sputter/game/timersystem.h>
#include "paddle.h"
#include "stage.h"
#include "ball.h"

struct GameState
{
    // Static accessor/mutator pair to grab the address all singleton-like
    static GameState* GetGameStateAddress();
    static void SetGameStateAddress(GameState* pGameState);

    GameState();
    
    enum class State 
    {
        Invalid = 0,
        Starting,
        Playing,
        Paused,
        Ended,
        Exiting,
        Restarting,
        MaxValue
    };

    sputter::render::Camera Camera;
    Ball             TheBall;
    Paddle           Player1Paddle;
    Paddle           Player2Paddle;
    Stage            Arena;
    int16_t          Player1Score = 0;          
    int16_t          Player2Score = 0;          
    uint8_t          WinningPlayer = 0;
    State            CurrentState = State::Invalid;

    using TimerSystem = sputter::game::TimerSystem;
    TimerSystem::TimerHandle CountdownTimerHandle = TimerSystem::kInvalidTimerHandle;
    uint8_t          CountdownTimeRemaining = 0;

private:
    static GameState* s_pGameState;
};
