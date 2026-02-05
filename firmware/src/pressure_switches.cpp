#include "pressure_switches.h"
#include "board_config.h"
#include <Arduino.h>

namespace tcm {

namespace {

// Debounce time: ignore rapid changes (noise, transients during shifting)
constexpr uint32_t kDebounceMs = 50;

} // namespace

void PressureSwitches::Init() {
  // Pins already configured with INPUT_PULLUP in main.cpp
  // Pressure switches are ground-switched (active LOW)
  // Pullup ensures HIGH when switch is open (no pressure)
  sw_a_ = false;
  sw_b_ = false;
  sw_c_ = false;
  position_ = ManualPosition::kUnknown;
  prev_position_ = ManualPosition::kUnknown;
  position_change_time_ms_ = 0;
}

void PressureSwitches::Update() {
  // Read switch states (active LOW: HIGH = open/no pressure, LOW = closed/pressure)
  // Note: Physical switches close to GND when pressure present
  // So digitalRead returns LOW when switch is activated
  sw_a_ = digitalRead(hw::kPinPressureSwitchA);
  sw_b_ = digitalRead(hw::kPinPressureSwitchB);
  sw_c_ = digitalRead(hw::kPinPressureSwitchC);
  
  // Decode position from switch pattern
  ManualPosition new_position = DecodePosition(sw_a_, sw_b_, sw_c_);
  
  // Debounce: only update if position changed AND debounce time elapsed
  if (new_position != position_) {
    uint32_t now_ms = millis();
    if (now_ms - position_change_time_ms_ >= kDebounceMs) {
      prev_position_ = position_;
      position_ = new_position;
      position_change_time_ms_ = now_ms;
    }
  }
}

ManualPosition PressureSwitches::DecodePosition(bool a, bool b, bool c) const {
  // OEM GM 4L60-E truth table (MegaShift documentation)
  // HIGH = switch open (no pressure), LOW = switch closed (pressure present)
  
  // ⚠️ WARNING: This table is based on OEM 4L60-E standard.
  // VERIFY ON YOUR TRANSMISSION before relying on this for safety-critical logic!
  // Testing guide: PRESSURE_SWITCH_VERIFICATION.md
  
  if (a == HIGH && b == LOW && c == HIGH) {
    return ManualPosition::kParkNeutral;  // Cannot distinguish Park from Neutral
  }
  if (a == LOW && b == LOW && c == HIGH) {
    return ManualPosition::kReverse;
  }
  if (a == LOW && b == HIGH && c == HIGH) {
    return ManualPosition::kFirst;
  }
  if (a == HIGH && b == HIGH && c == HIGH) {
    return ManualPosition::kSecond;
  }
  if (a == HIGH && b == HIGH && c == LOW) {
    return ManualPosition::kThird;
  }
  if (a == HIGH && b == LOW && c == LOW) {
    return ManualPosition::kFourth;
  }
  
  // Invalid pattern (all LOW is impossible, or non-standard valve body)
  return ManualPosition::kUnknown;
}

bool PressureSwitches::IsStable(uint32_t duration_ms) const {
  uint32_t now_ms = millis();
  uint32_t elapsed_ms = now_ms - position_change_time_ms_;
  return elapsed_ms >= duration_ms;
}

const char* ManualPositionToString(ManualPosition pos) {
  switch (pos) {
    case ManualPosition::kParkNeutral: return "P/N";
    case ManualPosition::kReverse:     return "R";
    case ManualPosition::kFirst:       return "1";
    case ManualPosition::kSecond:      return "2";
    case ManualPosition::kThird:       return "3";
    case ManualPosition::kFourth:      return "D";
    case ManualPosition::kUnknown:     return "???";
    default:                           return "ERR";
  }
}

} // namespace tcm
