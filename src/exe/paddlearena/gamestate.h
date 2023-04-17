#pragma once

#include <sputter/physics/rigidbodysubsystem.h>
#include <sputter/render/camera.h>
#include <sputter/game/timersystem.h>
#include <sputter/core/serializable.h>
#include "paddle.h"
#include "stage.h"
#include "ball.h"

struct GameState : public sputter::core::ISerializable
{
    // Static accessor/mutator pair to grab the address all singleton-like
    static GameState* GetGameStateAddress();
    static void SetGameStateAddress(GameState* pGameState);

    GameState();

    // Begin ISerializable
    virtual bool Serialize(void* pBuffer, size_t* pBytesWrittenOut, size_t maxBytes) override;
    virtual bool Deserialize(void* pBuffer, size_t* pBytesReadOut, size_t maxBytes) override;
    // End ISerializable
    
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
    uint32_t         Frame = 0;
    State            CurrentState = State::Invalid;

    using TimerSystem = sputter::game::TimerSystem;
    sputter::game::TimerHandle CountdownTimerHandle = sputter::game::kInvalidTimerHandle;
    uint8_t          CountdownTimeRemaining = 0;

private:
    static GameState* s_pGameState;
};

void ToString(GameState::State state, char* pBuffer);