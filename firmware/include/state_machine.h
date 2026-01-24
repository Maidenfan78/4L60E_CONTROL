#pragma once

#include <stdint.h>

namespace tcm {

enum class State : uint8_t {
 kInit,
 kIdle,
 kHoldGear,
 kShifting,
 kFailsafe,
};

struct StateContext {
 uint32_t now_ms = 0;
 bool sensor_fault = false;
};

class StateMachine {
 public:
 void Init();
 void Update(const StateContext& ctx);
 State state() const { return state_; }

 private:
 State state_ = State::kInit;
};

} // namespace tcm
