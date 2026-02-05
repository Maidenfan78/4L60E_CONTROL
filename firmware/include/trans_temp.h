#pragma once

#include <stdint.h>

namespace tcm {

/// @brief Transmission fluid temperature sensor handler
/// GM 4L60-E OEM NTC thermistor with 3.3kΩ pullup to 3.3V
class TransTempSensor {
 public:
  void Init();
  
  /// @brief Update temperature reading from ADC
  void Update();
  
  /// @brief Get current transmission temperature in Celsius
  /// @return Temperature in °C, or -128 if sensor fault
  int8_t GetTempCelsius() const { return temp_celsius_; }
  
  /// @brief Get current transmission temperature in Fahrenheit
  /// @return Temperature in °F, or -128 if sensor fault
  int16_t GetTempFahrenheit() const;
  
  /// @brief Check if sensor is valid
  /// @return true if sensor reading is within valid range
  bool IsValid() const { return is_valid_; }
  
  /// @brief Check if transmission is over-temperature
  /// @return true if temp exceeds safe operating limit
  bool IsOverTemp() const;
  
 private:
  int8_t temp_celsius_ = 25;  // Default to room temp
  bool is_valid_ = false;
  uint16_t raw_adc_ = 0;
  
  /// @brief Convert ADC value to temperature using lookup table
  /// @param adc_value 10-bit ADC reading (0-1023)
  /// @return Temperature in °C, or -128 if out of range
  int8_t AdcToTemp(uint16_t adc_value) const;
};

} // namespace tcm
