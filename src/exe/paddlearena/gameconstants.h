#pragma once

#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpconstants.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>

namespace gameconstants 
{
    // Rendering settings (TODO: 480p?)
    const float OrthoWidth = 1100.0f;
    const float OrthoHeight = 1000.0f;
    const float TargetAspectRatio = OrthoHeight / OrthoWidth;
    const float MinWidth   = 640;
    const float MinHeight = MinWidth * TargetAspectRatio;
    const auto InitialCameraPosition = sputter::math::FPVector3D(-500, 400, -200);

    // Game settings and constants

    // Arena config
    const auto ArenaDimensions = sputter::math::FPVector2D(800, 600);

    // Ball config
    const auto BallSpeed = sputter::math::FixedPoint(700);
    const auto PaddleSpeed = sputter::math::FixedPoint(400);
    const auto BallDimensions = sputter::math::FPVector2D(
            10, 10
        );
    const sputter::math::FPVector3D BallStartPosition = sputter::math::FPVector3D(
        sputter::math::FPZero,
        sputter::math::FPZero,
        -sputter::math::FPOne / sputter::math::FPTwo);

    const sputter::math::FPVector3D BallServePositionLeft = sputter::math::FPVector3D(
        sputter::math::FixedPoint(-300),
        sputter::math::FPZero,
        -sputter::math::FPOne / sputter::math::FPTwo);
    const sputter::math::FPVector2D BallServeDirectionLeft = sputter::math::FPVector2D(
        sputter::math::FPOne,
        sputter::math::FPOne
        );

    const sputter::math::FPVector3D BallServePositionRight = sputter::math::FPVector3D(
        sputter::math::FixedPoint(300),
        sputter::math::FPZero,
        -sputter::math::FPOne / sputter::math::FPTwo);

    const sputter::math::FPVector2D BallServeDirectionRight = sputter::math::FPVector2D(
        -sputter::math::FPOne,
        sputter::math::FPOne
        );

    const sputter::math::FPVector2D BallStartDirection = sputter::math::FPVector2D(
        -sputter::math::FPOne,
        sputter::math::FPOne
        );

    // Paddle config
    const sputter::math::FPVector3D P1PaddleStartPosition = sputter::math::FPVector3D(sputter::math::FixedPoint(-350), sputter::math::FPZero, -sputter::math::FPOne / sputter::math::FPTwo);
    const sputter::math::FPVector3D P2PaddleStartPosition = sputter::math::FPVector3D(sputter::math::FixedPoint(350), sputter::math::FPZero, -sputter::math::FPOne / sputter::math::FPTwo);

    const sputter::math::FixedPoint PaddleWidth = sputter::math::FixedPoint(20);
    const sputter::math::FixedPoint PaddleHeight = sputter::math::FixedPoint(80);

    // Game loop config
    const uint32_t StartCountdownSeconds = 3;

    // Game text config
    const uint32_t ScoreSize = 5;
    const uint32_t ScoreToWin = 9;
    const uint32_t WinMessageSize = 7;
    const uint32_t StartCountdownSize = 6;
    const uint32_t GameTitleSize = 5;

    const int32_t ScorePositionY = 305;
    const int32_t P1ScorePositionX = -300;
    const int32_t P2ScorePositionX = 200;

    const int32_t WinMessagePositionX = -350;
    const int32_t WinMessagePositionY = 0;

    const int32_t StartCountdownPositionX = -20;
    const int32_t StartCountdownPositionY = -130;

    const int32_t GameTitlePositionX = -380;
    const int32_t GameTitlePositionY = 10;

    // Strings
    static const char* GameTitleString = "PADDLEARENA";
}