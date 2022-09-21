#include "events.h"

using namespace sputter::ui;

uint8_t sputter::ui::EventToParameter(Event event) { return static_cast<uint8_t>(event); }

// This has the possibility to be dangerous, but our event calls are completely synchronous
// so risk is minimal.
void* sputter::ui::KeyPointerToParameter(Key* pKey) { return static_cast<void*>(pKey); }