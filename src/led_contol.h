#pragma once

#include <array>

#include <microsDelay.h>

#include "constants.h"
#include "curve.h"
#include "led.h"
#include "temperture.h"

namespace LB {
const uint16_t LED_UPDATE_PERIOD_US = 1000;
const CalculatorDimension MAX_SPEED = 1 << 10;
const CalculatorDimension MAX_TIME_STEP = 100;
const CalculatorDimension TIME_STEP_INCREMENT =
    MAX_TIME_STEP / (CalculatorDimension)LED_UPDATE_PERIOD_US;

class LEDControl {
public:
  void init(LED &led);
  std::array<Curve, Constants::NUM_LED_CHANNELS> &get_curves() {
    return curves_;
  };
  const std::array<Curve, Constants::NUM_LED_CHANNELS> &
  get_curves_const() const {
    return curves_;
  };
  void set_temperature_sensor(Temperature &sensor);
  void set_time_controlled(bool time_controlled) {
    time_controlled_ = time_controlled;
  };
  const inline bool is_time_controlled() const { return time_controlled_; };
  inline void set_speed(CalculatorDimension speed) { speed_ = speed; };
  inline CalculatorDimension get_speed() const { return speed_; };
  void loop();

  size_t serialize(ConfigMemType &buff, size_t offset) const;
  size_t derialize(ConfigMemType &buff, size_t offset);

private:
  std::array<Curve, Constants::NUM_LED_CHANNELS> curves_;
  LED *led_ = nullptr;
  Temperature *sensor_ = nullptr;
  microsDelay update_delay_;
  bool time_controlled_ = true;
  CalculatorDimension speed_ = 0.25f;
  CalculatorDimension current_time_step_ = 0;
};
} // namespace LB
