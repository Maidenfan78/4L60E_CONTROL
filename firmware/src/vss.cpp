#include <Arduino.h>

#include board_config.h
#include vss.h

namespace tcm {

void VssSensor::Init() {
 sample_ = {};
}

void VssSensor::Update() {
 // TODO: Capture VSS pulses via interrupt and compute period.
}

} // namespace tcm
