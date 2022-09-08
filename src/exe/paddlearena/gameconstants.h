#pragma once

#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpconstants.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>

const sputter::math::FixedPoint kGameConstantsBallSpeed = sputter::math::FixedPoint(700);
const sputter::math::FixedPoint kGameConstantsPaddleSpeed = sputter::math::FixedPoint(400);
const sputter::math::FixedPoint kGameConstantPaddleWidth = sputter::math::FixedPoint(20);
const sputter::math::FixedPoint kGameConstantPaddleHeight = sputter::math::FixedPoint(80);

const sputter::math::FPVector3D kGameConstantP1PaddleStartPosition = sputter::math::FPVector3D(sputter::math::FixedPoint(-350), sputter::math::FPZero, -sputter::math::FPOne / sputter::math::FPTwo);
const sputter::math::FPVector3D kGameConstantP2PaddleStartPosition = sputter::math::FPVector3D(sputter::math::FixedPoint(350), sputter::math::FPZero, -sputter::math::FPOne / sputter::math::FPTwo);

const sputter::math::FPVector2D kGameConstantsBallDimensions = sputter::math::FPVector2D(
        10, 10
    );
const sputter::math::FPVector3D kGameConstantsBallStartPosition = sputter::math::FPVector3D(
    sputter::math::FPZero,
    sputter::math::FPZero,
    -sputter::math::FPOne / sputter::math::FPTwo);

const sputter::math::FPVector3D kGameConstantsBallServePositionLeft = sputter::math::FPVector3D(
    sputter::math::FixedPoint(-300),
    sputter::math::FPZero,
    -sputter::math::FPOne / sputter::math::FPTwo);
const sputter::math::FPVector2D kGameConstantsBallServeDirectionLeft = sputter::math::FPVector2D(
    sputter::math::FPOne,
    sputter::math::FPOne
    );

const sputter::math::FPVector3D kGameConstantsBallServePositionRight = sputter::math::FPVector3D(
    sputter::math::FixedPoint(300),
    sputter::math::FPZero,
    -sputter::math::FPOne / sputter::math::FPTwo);

const sputter::math::FPVector2D kGameConstantsBallServeDirectionRight = sputter::math::FPVector2D(
    -sputter::math::FPOne,
    sputter::math::FPOne
    );

// This will eventually be dynamic
const sputter::math::FPVector2D kGameConstantsBallStartDirection = sputter::math::FPVector2D(
    -sputter::math::FPOne,
    sputter::math::FPOne
    );

const uint32_t kGameConstantsScoreSize = 5;
const uint32_t kGameConstantsScoreToWin = 9;

const uint32_t kGameConstantsWinMessageSize = 7;