#include state_machine.h

namespace tcm {

void StateMachine::Init() {
 state_ = State::kInit;
}

void StateMachine::Update(const StateContext& ctx) {
 if (ctx.sensor_fault) {
 state_ = State::kFailsafe;
 return;
 }

 switch (state_) {
 case State::kInit:
 state_ = State::kIdle;
 break;
 case State::kIdle:
 case State::kHoldGear:
 case State::kShifting:
 case State::kFailsafe:
 default:
 break;
 }
}

} // namespace tcm
