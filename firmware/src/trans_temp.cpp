#include "trans_temp.h"
#include "board_config.h"
#include <Arduino.h>

namespace tcm {

namespace {

// Lookup table for GM 4L60-E temp sensor with 3.3kΩ pullup
// Source: GM eSI (Electronic Service Information)
// Confirmed resistance: 3088-3942Ω @ 68°F, 159-198Ω @ 212°F
// ADC values are 10-bit (0-1023) for Teensy 4.1

struct TempCalPoint {
  uint16_t adc;      // ADC value (10-bit)
  int8_t temp_c;     // Temperature in Celsius
};

// Lookup table: ADC value → Temperature (°C)
// Sorted by ADC value (descending = coldest first)
constexpr TempCalPoint kTempTable[] = {
  { 600,  15 },  // Very cold (winter startup)
  { 523,  20 },  // Room temp (68°F)
  { 450,  25 },  // Slightly warm
  { 380,  30 },  // Warm
  { 320,  35 },  
  { 270,  40 },  
  { 230,  45 },  
  { 195,  50 },  
  { 165,  55 },  
  { 140,  60 },  
  { 120,  65 },  
  { 100,  70 },  
  {  85,  75 },  
  {  70,  80 },  
  {  65,  88 },  // Normal operating temp (~190°F)
  {  52, 100 },  // Hot (212°F)
  {  40, 110 },  // Very hot (230°F)
  {  30, 120 },  // Danger zone (248°F)
  {  20, 130 },  // Critical over-temp (266°F)
};

constexpr size_t kTempTableSize = sizeof(kTempTable) / sizeof(kTempTable[0]);

// Temperature thresholds
constexpr int8_t kTempMinValid = 0;      // Below 0°C = sensor fault (frozen?)
constexpr int8_t kTempMaxValid = 140;    // Above 140°C = sensor fault (shorted?)
constexpr int8_t kTempOverTemp = 130;    // 130°C (266°F) = over-temp warning
constexpr uint16_t kAdcMinValid = 15;    // ADC too low = shorted to ground
constexpr uint16_t kAdcMaxValid = 650;   // ADC too high = open circuit

} // namespace

void TransTempSensor::Init() {
  pinMode(hw::kPinTransTemp, INPUT);
  temp_celsius_ = 25;  // Default to room temp on boot
  is_valid_ = false;
}

void TransTempSensor::Update() {
  // Read ADC (10-bit, 0-1023)
  raw_adc_ = analogRead(hw::kPinTransTemp);
  
  // Validate ADC range
  if (raw_adc_ < kAdcMinValid || raw_adc_ > kAdcMaxValid) {
    is_valid_ = false;
    temp_celsius_ = -128;  // Sentinel value for fault
    return;
  }
  
  // Convert ADC to temperature
  temp_celsius_ = AdcToTemp(raw_adc_);
  
  // Validate temperature range
  if (temp_celsius_ < kTempMinValid || temp_celsius_ > kTempMaxValid) {
    is_valid_ = false;
    temp_celsius_ = -128;
    return;
  }
  
  is_valid_ = true;
}

int8_t TransTempSensor::AdcToTemp(uint16_t adc_value) const {
  // Handle edge cases
  if (adc_value >= kTempTable[0].adc) {
    // Colder than coldest calibration point
    return kTempTable[0].temp_c;
  }
  if (adc_value <= kTempTable[kTempTableSize - 1].adc) {
    // Hotter than hottest calibration point
    return kTempTable[kTempTableSize - 1].temp_c;
  }
  
  // Linear interpolation between calibration points
  for (size_t i = 0; i < kTempTableSize - 1; ++i) {
    if (adc_value >= kTempTable[i + 1].adc && adc_value <= kTempTable[i].adc) {
      // Found the bracketing points
      uint16_t adc_low = kTempTable[i + 1].adc;
      uint16_t adc_high = kTempTable[i].adc;
      int8_t temp_low = kTempTable[i + 1].temp_c;
      int8_t temp_high = kTempTable[i].temp_c;
      
      // Linear interpolation: temp = temp_high + (adc - adc_high) * (temp_low - temp_high) / (adc_low - adc_high)
      // Note: adc_high > adc_low (ADC decreases as temp increases)
      int32_t temp = temp_high + ((int32_t)(adc_value - adc_high) * (temp_low - temp_high)) / (adc_low - adc_high);
      
      return (int8_t)temp;
    }
  }
  
  // Should never reach here if table is correct
  return -128;
}

int16_t TransTempSensor::GetTempFahrenheit() const {
  if (!is_valid_) {
    return -128;
  }
  // °F = (°C × 9/5) + 32
  return (int16_t)((temp_celsius_ * 9) / 5 + 32);
}

bool TransTempSensor::IsOverTemp() const {
  if (!is_valid_) {
    return true;  // Treat sensor fault as over-temp (safe failure mode)
  }
  return temp_celsius_ >= kTempOverTemp;
}

} // namespace tcm
