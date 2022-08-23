#pragma once

#include <sputter/math/fixedpoint.h>
#include <sputter/math/fpconstants.h>
#include <sputter/math/fpvector2d.h>
#include <sputter/math/fpvector3d.h>

const sputter::math::FixedPoint kGameConstantsBallSpeed = sputter::math::FixedPoint(800);
const sputter::math::FixedPoint kGameConstantsPaddleSpeed = sputter::math::FixedPoint(400);

const sputter::math::FPVector2D kGameConstantsBallDimensions = sputter::math::FPVector2D(
        10, 10
    );
const sputter::math::FPVector3D kGameConstantsBallStartPosition = sputter::math::FPVector3D(
    sputter::math::FPZero,
    sputter::math::FPZero,
    -sputter::math::FPOne / sputter::math::FPTwo);

// This will eventually be dynamic
const sputter::math::FPVector2D kGameConstantsBallStartDirection = sputter::math::FPVector2D(
    -sputter::math::FPOne,
    sputter::math::FPOne
    );