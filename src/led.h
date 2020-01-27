#ifndef __LED_H__
#define __LED_H__

#include <array>
#include <tuple>

#include "constants.h"
#include "utils.h"

namespace LB {
// static const Point example_point({99, 75}, {99, 75});

const uint16_t MIN_BRIGHTNESS_DAC_LEVEL = 2650;
const uint16_t MAX_BRIGHTNESS_DAC_LEVEL = 3875;
const float MIN_BRIGHTNESS = 0.0f;
const float MAX_BRIGHTNESS = 100.0f;

class LED {
public:
  void init(SetDacFunc set_r_channel_func, SetDacFunc set_g_channel_func,
            SetDacFunc set_b_channel_func);
  void set_r_brightness(float brightness);
  void set_g_brightness(float brightness);
  void set_b_brightness(float brightness);
  float get_r_brightness();
  float get_g_brightness();
  float get_b_brightness();

private:
  SetDacFunc set_r_dac_ = nullptr;
  SetDacFunc set_g_dac_ = nullptr;
  SetDacFunc set_b_dac_ = nullptr;

  float r_brightness_;
  float g_brightness_;
  float b_brightness_;

  std::array<Point, LB::Constants::POINTS_PER_CURVE> points;
};
} // namespace LB

#endif
