#include <Arduino.h>

#include board_config.h
#include epc_control.h

namespace tcm {

void EpcController::Init() {
  analogWrite(hw::kPinEPC, config_.failsafe_duty);
}

void EpcController::ApplyFailsafe() {
  analogWrite(hw::kPinEPC, config_.failsafe_duty);
}

void EpcController::Update() {
 // TODO: Replace with TPS-based control.
}

} // namespace tcm
