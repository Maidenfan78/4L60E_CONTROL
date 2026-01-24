#pragma once

#include <stdint.h>

namespace tcm {

struct VssSample {
 uint32_t last_tick_ms = 0;
 uint32_t period_us = 0;
 bool valid = false;
};

class VssSensor {
 public:
 void Init();
 void Update();
 VssSample sample() const { return sample_; }

 private:
 VssSample sample_{};
};

} // namespace tcm
