#pragma once

#include <mbed.h>

class ClockIn {
public:
  ClockIn();

  void tick(int clock);

  float position() const {
    return period_ ? static_cast<float>(counter_)/period_ : 0;
  }

  uint32_t period() const {
    return period_;
  }

  uint32_t counter() const {
    return counter_;
  }

  bool isRising() const {
    return value_ && !prevValue_;
  }

  bool isFalling() const {
    return !value_ && prevValue_;
  }

  uint32_t count() const {
    return periodCount_;
  }

private:
  uint32_t period_;
  uint32_t counter_;
  bool prevValue_;
  bool value_;
  uint32_t periodCount_;
};
