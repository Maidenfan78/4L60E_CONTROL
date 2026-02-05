#pragma once

#include <stdint.h>

namespace tcm {

/// @brief Manual valve position detected by pressure switch manifold
/// @note Based on OEM GM 4L60-E truth table (MegaShift documentation)
/// @warning ⚠️  MUST BE VERIFIED ON YOUR TRANSMISSION BEFORE USE
/// @warning See PRESSURE_SWITCH_VERIFICATION.md for testing procedure
enum class ManualPosition : uint8_t {
  kParkNeutral = 0,  // HIGH, LOW, HIGH (cannot distinguish P from N)
  kReverse,          // LOW, LOW, HIGH
  kFirst,            // LOW, HIGH, HIGH (D1 position)
  kSecond,           // HIGH, HIGH, HIGH (D2 position)
  kThird,            // HIGH, HIGH, LOW (D3 position)
  kFourth,           // HIGH, LOW, LOW (D/D4 position)
  kUnknown,          // Invalid pattern (wiring fault or non-standard valve body)
};

/// @brief Pressure switch manifold reader
/// Reads the 3-switch manifold to determine manual valve (shifter) position
/// @note These indicate SHIFTER position, NOT actual transmission gear
class PressureSwitches {
 public:
  void Init();
  
  /// @brief Update pressure switch readings
  void Update();
  
  /// @brief Get current manual valve position
  /// @return Detected shifter position
  ManualPosition GetPosition() const { return position_; }
  
  /// @brief Get raw switch states
  /// @param[out] sw_a Switch A state (true = HIGH = no pressure)
  /// @param[out] sw_b Switch B state
  /// @param[out] sw_c Switch C state
  void GetRawSwitches(bool& sw_a, bool& sw_b, bool& sw_c) const {
    sw_a = sw_a_;
    sw_b = sw_b_;
    sw_c = sw_c_;
  }
  
  /// @brief Check if switch readings are valid
  /// @return false if impossible pattern detected (wiring fault)
  bool IsValid() const { return position_ != ManualPosition::kUnknown; }
  
  /// @brief Check if position has been stable for a duration
  /// @param duration_ms Minimum time position must be stable (milliseconds)
  /// @return true if position unchanged for duration_ms
  bool IsStable(uint32_t duration_ms) const;
  
 private:
  bool sw_a_ = false;
  bool sw_b_ = false;
  bool sw_c_ = false;
  ManualPosition position_ = ManualPosition::kUnknown;
  ManualPosition prev_position_ = ManualPosition::kUnknown;
  uint32_t position_change_time_ms_ = 0;
  
  /// @brief Decode switch pattern to manual position
  /// @param a Switch A state (true = HIGH)
  /// @param b Switch B state
  /// @param c Switch C state
  /// @return Decoded position
  ManualPosition DecodePosition(bool a, bool b, bool c) const;
};

/// @brief Convert ManualPosition to human-readable string
/// @param pos Manual position enum
/// @return Null-terminated string (for Serial logging)
const char* ManualPositionToString(ManualPosition pos);

} // namespace tcm
