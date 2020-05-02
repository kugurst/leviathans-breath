#include "led.h"

#include <cmath>

#include "pwm_utils.hpp"

namespace LB {
void LED::init(uint8_t gd_pin_r, uint8_t gd_pin_g, uint8_t gd_pin_b) {
  gd_pin_r_ = gd_pin_r;
  gd_pin_g_ = gd_pin_g;
  gd_pin_b_ = gd_pin_b;
}

void LED::begin() {
  init_pwm_pin(gd_pin_r_, 0);
  init_pwm_pin(gd_pin_g_, 0);
  init_pwm_pin(gd_pin_b_, 0);
}

void LED::set_r_brightness(float brightness) {
  r_brightness_ = fmin(MAX_BRIGHTNESS, fmax(MIN_BRIGHTNESS, brightness));
  const auto adjusted_brigtness =
      sq(r_brightness_ / MAX_BRIGHTNESS) * (float)MAX_PWM_RAW;
  analogWrite(gd_pin_r_, adjusted_brigtness);
}

void LED::set_g_brightness(float brightness) {
  g_brightness_ = fmin(MAX_BRIGHTNESS, fmax(MIN_BRIGHTNESS, brightness));
  const auto adjusted_brigtness =
      sq(b_brightness_ / MAX_BRIGHTNESS) * (float)MAX_PWM_RAW;
  analogWrite(gd_pin_g_, adjusted_brigtness);
}

void LED::set_b_brightness(float brightness) {
  b_brightness_ = fmin(MAX_BRIGHTNESS, fmax(MIN_BRIGHTNESS, brightness));
  const auto adjusted_brigtness =
      sq(b_brightness_ / MAX_BRIGHTNESS) * (float)MAX_PWM_RAW;
  analogWrite(gd_pin_b_, adjusted_brigtness);
}
} // namespace LB
