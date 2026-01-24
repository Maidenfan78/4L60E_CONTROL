#pragma once

#include <stdint.h>

namespace tcm {

struct EpcConfig {
  uint16_t failsafe_duty = 0;
};

class EpcController {
 public:
 void Init();
 void ApplyFailsafe();
 void Update();

 private:
 EpcConfig config_{};
};

} // namespace tcm
