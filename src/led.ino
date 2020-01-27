#include "led.h"

#include "math.h"

namespace LB {
void LED::init(SetDacFunc set_r_channel_func, SetDacFunc set_g_channel_func,
               SetDacFunc set_b_channel_func) {
  set_r_dac_ = set_r_channel_func;
  set_g_dac_ = set_g_channel_func;
  set_b_dac_ = set_b_channel_func;
}

void LED::set_r_brightness(float brightness) {
  r_brightness_ = fmin(MAX_BRIGHTNESS, fmax(MIN_BRIGHTNESS, brightness));
  const auto dac_level =
      sqrt(r_brightness_ / MAX_BRIGHTNESS) *
          (MAX_BRIGHTNESS_DAC_LEVEL - MIN_BRIGHTNESS_DAC_LEVEL) +
      MIN_BRIGHTNESS_DAC_LEVEL;
  set_r_dac_(dac_level);
}

void LED::set_g_brightness(float brightness) {
  g_brightness_ = fmin(MAX_BRIGHTNESS, fmax(MIN_BRIGHTNESS, brightness));
  const auto dac_level =
      sqrt(g_brightness_ / MAX_BRIGHTNESS) *
          (MAX_BRIGHTNESS_DAC_LEVEL - MIN_BRIGHTNESS_DAC_LEVEL) +
      MIN_BRIGHTNESS_DAC_LEVEL;
  set_g_dac_(dac_level);
}

void LED::set_b_brightness(float brightness) {
  b_brightness_ = fmin(MAX_BRIGHTNESS, fmax(MIN_BRIGHTNESS, brightness));
  const auto dac_level =
      sqrt(b_brightness_ / MAX_BRIGHTNESS) *
          (MAX_BRIGHTNESS_DAC_LEVEL - MIN_BRIGHTNESS_DAC_LEVEL) +
      MIN_BRIGHTNESS_DAC_LEVEL;
  set_b_dac_(dac_level);
}
} // namespace LB
