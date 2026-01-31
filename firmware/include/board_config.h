#pragma once

#include <stdint.h>

// Central pin map and hardware constants.

namespace hw {

constexpr uint8_t kPinSSA = 2;
constexpr uint8_t kPinSSB = 3;
constexpr uint8_t kPinTCC = 4;
constexpr uint8_t kPinEPC = 5;
constexpr uint8_t kPinTCCPWM = 12;

constexpr uint8_t kPinRange1 = 6;
constexpr uint8_t kPinRange2 = 7;
constexpr uint8_t kPinRange3 = 8;
constexpr uint8_t kPinRangeD = 9;

constexpr uint8_t kPinBrake = 10;
constexpr uint8_t kPinWatchdogEnable = 11;

constexpr uint8_t kPinTPS = 14; // A0
constexpr uint8_t kPinTransTemp = 15; // A1
constexpr uint8_t kPinLinePressure = 16; // A2

constexpr uint8_t kPinVSS = 18;
constexpr uint8_t kPinRPM = 19;

// Pressure switch manifold (gear confirmation)
constexpr uint8_t kPinPressureSwitchA = 20;
constexpr uint8_t kPinPressureSwitchB = 21;
constexpr uint8_t kPinPressureSwitchC = 22;

} // namespace hw
