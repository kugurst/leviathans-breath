#ifndef __LED_H__
#define __LED_H__

#include <array>
#include <tuple>

#include "constants.hpp"
#include "utils.h"

namespace LB {
const float MIN_BRIGHTNESS = 0.0f;
const float MAX_BRIGHTNESS = 100.0f;

class LED {
public:
  void init(uint8_t gd_pin_r, uint8_t gd_pin_g, uint8_t gd_pin_b);

  void begin();
  
  void set_r_brightness(float brightness);
  void set_g_brightness(float brightness);
  void set_b_brightness(float brightness);
  const inline float get_r_brightness() const { return r_brightness_; };
  const inline float get_g_brightness() const { return g_brightness_; };
  const inline float get_b_brightness() const { return b_brightness_; };

private:
  uint8_t gd_pin_r_ = 0;
  uint8_t gd_pin_g_ = 0;
  uint8_t gd_pin_b_ = 0;

  float r_brightness_;
  float g_brightness_;
  float b_brightness_;

  std::array<Point, LB::Constants::POINTS_PER_CURVE> points;
};
} // namespace LB

#endif
