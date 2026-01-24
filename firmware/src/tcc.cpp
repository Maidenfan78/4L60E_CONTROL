#include <Arduino.h>

#include board_config.h
#include tcc.h

namespace tcm {

void TccController::Init() {
 digitalWrite(hw::kPinTCC, LOW);
}

void TccController::Update() {
 // TODO: Apply enable logic and command output.
}

void TccController::Disable() {
 digitalWrite(hw::kPinTCC, LOW);
}

} // namespace tcm
