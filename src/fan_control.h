#pragma once

#include <microsDelay.h>

#include "constants.h"
#include "curve.h"
#include "fan.h"
#include "temperture.h"

namespace LB {
const uint16_t UPDATE_PERIOD_US = 10000;

class FanControl {
public:
  void init(Fan &fan, Temperature &sensor);
  Curve &get_curve() { return temperature_curve_; };
  const Curve &get_curve_const() const { return temperature_curve_; };
  void change_temperature_sensor(Temperature &new_sensor) {
    sensor_ = &new_sensor;
  };
  void loop();

  size_t serialize(ConfigMemType& buff, size_t offset) const;
  size_t derialize(ConfigMemType& buff, size_t offset);

private:
  Curve temperature_curve_;
  Fan *fan_ = nullptr;
  Temperature *sensor_ = nullptr;
  microsDelay update_delay_;
};
} // namespace LB
