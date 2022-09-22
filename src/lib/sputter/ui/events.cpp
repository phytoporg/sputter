#include "events.h"

using namespace sputter::ui;

uint8_t sputter::ui::EventCodeToParameter(EventCode event) { return static_cast<uint8_t>(event); }
EventCode sputter::ui::ParameterToEventCode(uint8_t param) { return static_cast<EventCode>(param); }

// This has the possibility to be dangerous, but our event calls are completely synchronous
// so risk is minimal.
void* sputter::ui::KeyPointerToParameter(Key* pKey) { return static_cast<void*>(pKey); }
