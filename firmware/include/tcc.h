#pragma once

#include <stdint.h>

namespace tcm {

struct TccConfig {
 bool enabled = false;
};

class TccController {
 public:
 void Init();
 void Update();
 void Disable();

 private:
 TccConfig config_{};
};

} // namespace tcm
