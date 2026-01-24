#include <Arduino.h>

#include board_config.h
#include epc_control.h
#include state_machine.h
#include tcc.h
#include vss.h

namespace {

tcm::EpcController epc;
tcm::VssSensor vss;
tcm::TccController tcc;
tcm::StateMachine state_machine;

void ConfigurePins() {
 pinMode(hw::kPinSSA, OUTPUT);
 pinMode(hw::kPinSSB, OUTPUT);
 pinMode(hw::kPinTCC, OUTPUT);
 pinMode(hw::kPinEPC, OUTPUT);

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
 analogWrite(hw::kPinEPC, 255); // Max duty to protect clutches.
}

} // namespace

void setup() {
 ConfigurePins();
 ApplySafeOutputs();
 epc.Init();
 vss.Init();
 tcc.Init();
 state_machine.Init();
 Serial.begin(115200);
 while (!Serial && millis() < 3000) {
 delay(10);
 }
 Serial.println(4L60E TCM boot);
}

void loop() {
 // TODO: Replace with state machine execution.
 static uint32_t last_log_ms = 0;
 const uint32_t now_ms = millis();
 vss.Update();
 tcc.Update();
 epc.Update();
 tcm::StateContext ctx{};
 ctx.now_ms = now_ms;
 state_machine.Update(ctx);
 if (now_ms - last_log_ms >= 1000) {
 last_log_ms = now_ms;
 Serial.println(TCM heartbeat);
 }
 delay(10);
}
