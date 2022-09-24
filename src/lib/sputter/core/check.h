#pragma once

// TODO: The dependencies typically pulled from system.h really ought to be placed in
// a "logging.h" of some kind.
#include <sputter/system/system.h>

#define RELEASE_CHECK(Condition, FailureText) if (!(Condition)) { system::LogAndFail(FailureText); }