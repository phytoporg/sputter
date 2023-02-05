#pragma once

#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpconstants.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>
#include <sputter/math/vector2i.h>

#include <sputter/render/color.h>

namespace gameconstants 
{
    // Rendering settings
    const float OrthoWidth = 1100.0f;
    const float OrthoHeight = 1000.0f;
    const float TargetAspectRatio = OrthoHeight / OrthoWidth;
    const float MinWidth   = 640;
    const float MinHeight = MinWidth * TargetAspectRatio;

    // Center the camera over the center of the plane
    const auto InitialCameraPosition = sputter::math::FPVector3D(-static_cast<int>(OrthoWidth) / 2, static_cast<int>(OrthoHeight) / 2, -200);

    // Game settings and constants
    // Arena config
    const auto ArenaDimensions = sputter::math::FPVector2D(800, 600);

    // Ball config
    const auto BallSpeed = sputter::math::FixedPoint(850);
    const auto PaddleSpeed = sputter::math::FixedPoint(500);
    const auto PaddleDashSpeed = sputter::math::FixedPoint(2000);
    const auto PaddleHitReferencePointDistance = sputter::math::FixedPoint(5);
    const auto BallDimensions = sputter::math::FPVector2D(
            10, 10
        );
    const auto BallStartPosition = sputter::math::FPVector3D(
        sputter::math::FPZero,
        sputter::math::FPZero,
        -sputter::math::FPOneHalf);

    const auto BallServePositionLeft = sputter::math::FPVector3D(
        sputter::math::FixedPoint(-300),
        sputter::math::FPZero,
        -sputter::math::FPOneHalf);
    const auto BallServeDirectionLeft = sputter::math::FPVector2D(
        sputter::math::FPOne,
        sputter::math::FPOne
        );

    const auto BallServePositionRight = sputter::math::FPVector3D(
        sputter::math::FixedPoint(300),
        sputter::math::FPZero,
        -sputter::math::FPOneHalf);

    const auto BallServeDirectionRight = sputter::math::FPVector2D(
        -sputter::math::FPOne,
        sputter::math::FPOne
        );

    const auto BallStartDirection = sputter::math::FPVector2D(
        -sputter::math::FPOne,
        sputter::math::FPOne
        );

    // Paddle config
    const auto PaddleStartPositionDistanceX = sputter::math::FixedPoint(350);
    const auto PaddleStartPositionDistanceY = sputter::math::FPZero;

    const auto P1PaddleStartPosition = sputter::math::FPVector3D(-PaddleStartPositionDistanceX, PaddleStartPositionDistanceY, -sputter::math::FPOneHalf);
    const auto P2PaddleStartPosition = sputter::math::FPVector3D(PaddleStartPositionDistanceX, PaddleStartPositionDistanceY, -sputter::math::FPOneHalf);

    const auto PaddleWidth = sputter::math::FixedPoint(20);
    const auto PaddleHeight = sputter::math::FixedPoint(80);

    // Game loop config
    const uint32_t StartCountdownSeconds = 3;

    // Game text config
    const uint32_t ScoreSize = 5;
    // const uint32_t ScoreToWin = 9;
    const uint32_t ScoreToWin = 3;
    const uint32_t WinningScoreAdvantage = 2;
    const uint32_t WinMessageSize = 7;
    const uint32_t StartCountdownSize = 6;
    const uint32_t GetReadySize = 5;

    const int32_t ScorePositionY = 305;
    const int32_t P1ScorePositionX = -300;
    const int32_t P2ScorePositionX = 200;

    const int32_t WinMessagePositionX = -350;
    const int32_t WinMessagePositionY = 0;

    const int32_t StartCountdownPositionX = -20;
    const int32_t StartCountdownPositionY = -110;

    const int32_t GetReadyPositionX = -280;
    const int32_t GetReadyPositionY = 30;

    // Strings
    static const char* GetReadyString = "GET READY";
    static const char* GameTitleString = "PADDLEARENA";

    // Main menu constants
    const int32_t VersusAiButtonPositionX = -100;
    const int32_t VersusAiButtonPositionY = -100;

    const uint32_t MainMenuButtonDimensionX = 200;
    const uint32_t MainMenuButtonDimensionY = 50;
    const uint32_t MainMenuButtonMarginTop = 100;
    const uint32_t MainMenuButtonBorderSize = 3;
    const auto MainMenuButtonBorderColor = sputter::render::Color::WHITE;

    const int32_t MainMenuGameTitlePositionX = -380;
    const int32_t MainMenuGameTitlePositionY = 100;
    const int32_t MainMenuGameTitleSize = 5;

    // In-game UI constants
    const auto GameEndModalPosition = sputter::math::Vector2i(-200, -200);
    const auto GameEndModalDimensions = sputter::math::Vector2i(400, 400);
    const auto GameEndModalButtonDimensions = sputter::math::Vector2i(120, 50);
    static const char* GameEndRestartButtonText = "RESTART";
    static const char* GameEndExitButtonText = "EXIT";
}
