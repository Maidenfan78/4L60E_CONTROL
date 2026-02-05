#include <Arduino.h>

#include "board_config.h"
#include "epc_control.h"
#include "state_machine.h"
#include "tcc.h"
#include "vss.h"
#include "trans_temp.h"
#include "pressure_switches.h"

namespace {

tcm::EpcController epc;
tcm::VssSensor vss;
tcm::TccController tcc;
tcm::StateMachine state_machine;
tcm::TransTempSensor trans_temp;
tcm::PressureSwitches pressure_switches;

void ConfigurePins() {
 pinMode(hw::kPinSSA, OUTPUT);
 pinMode(hw::kPinSSB, OUTPUT);
 pinMode(hw::kPinTCC, OUTPUT);
 pinMode(hw::kPinEPC, OUTPUT);
 // TODO Phase 4: pinMode(hw::kPinTCCPWM, OUTPUT); for progressive TCC apply

 pinMode(hw::kPinRange1, INPUT_PULLUP);
 pinMode(hw::kPinRange2, INPUT_PULLUP);
 pinMode(hw::kPinRange3, INPUT_PULLUP);
 pinMode(hw::kPinRangeD, INPUT_PULLUP);

 pinMode(hw::kPinBrake, INPUT_PULLUP);
 pinMode(hw::kPinWatchdogEnable, INPUT_PULLUP);

 pinMode(hw::kPinVSS, INPUT);
 pinMode(hw::kPinRPM, INPUT);
}

void ApplySafeOutputs() {
  digitalWrite(hw::kPinSSA, LOW);
  digitalWrite(hw::kPinSSB, LOW);
  digitalWrite(hw::kPinTCC, LOW);
  analogWrite(hw::kPinEPC, 0); // 0% duty = max pressure (project assumption).
}

} // namespace

void setup() {
 ConfigurePins();
 ApplySafeOutputs();
 epc.Init();
 vss.Init();
 tcc.Init();
 state_machine.Init();
 trans_temp.Init();
 pressure_switches.Init();
 
 Serial.begin(115200);
 while (!Serial && millis() < 3000) {
 delay(10);
 }
 Serial.println("4L60E TCM boot");
 Serial.println("⚠️  WARNING: Pressure switch truth table NOT VERIFIED");
 Serial.println("    Verify switch patterns before relying on gear confirmation");
}

void loop() {
 // TODO: Replace with state machine execution.
 static uint32_t last_log_ms = 0;
 const uint32_t now_ms = millis();
 
 // Update all sensors
 vss.Update();
 tcc.Update();
 epc.Update();
 trans_temp.Update();
 pressure_switches.Update();
 
 // Prepare state machine context
 tcm::StateContext ctx{};
 ctx.now_ms = now_ms;
 ctx.sensor_fault = !trans_temp.IsValid() || !pressure_switches.IsValid();
 
 state_machine.Update(ctx);
 
 // Periodic telemetry logging (every 1 second)
 if (now_ms - last_log_ms >= 1000) {
  last_log_ms = now_ms;
  
  // Log temperature
  if (trans_temp.IsValid()) {
   Serial.print("Trans Temp: ");
   Serial.print(trans_temp.GetTempCelsius());
   Serial.print("°C (");
   Serial.print(trans_temp.GetTempFahrenheit());
   Serial.print("°F)");
   if (trans_temp.IsOverTemp()) {
    Serial.print(" ⚠️  OVER-TEMP");
   }
   Serial.println();
  } else {
   Serial.println("Trans Temp: FAULT");
  }
  
  // Log pressure switches
  Serial.print("Manual Position: ");
  Serial.print(tcm::ManualPositionToString(pressure_switches.GetPosition()));
  
  bool sw_a, sw_b, sw_c;
  pressure_switches.GetRawSwitches(sw_a, sw_b, sw_c);
  Serial.print(" (A:");
  Serial.print(sw_a ? "H" : "L");
  Serial.print(" B:");
  Serial.print(sw_b ? "H" : "L");
  Serial.print(" C:");
  Serial.print(sw_c ? "H" : "L");
  Serial.print(")");
  
  if (!pressure_switches.IsValid()) {
   Serial.print(" ⚠️  INVALID PATTERN");
  }
  Serial.println();
  
  Serial.println("---");
 }
 
 delay(10);
}
