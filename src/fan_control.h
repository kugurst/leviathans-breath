#pragma once

#include <microsDelay.h>

#include "constants.hpp"
#include "curve.h"
#include "fan.h"
#include "temperture.h"

namespace LB {
const uint16_t UPDATE_PERIOD_US = 10000;

class FanControl {
public:
  void init(Fan &fan, Temperature &sensor);
  inline Curve &get_curve() { return temperature_curve_; };
  const inline Curve &get_curve_const() const { return temperature_curve_; };
  void set_temperature_sensor(Temperature &new_sensor, int8_t new_sensor_idx);
  const inline int8_t get_temperature_sensor() const { return sensor_idx_; };
  void loop();

  size_t serialize(ConfigMemType& buff, size_t offset) const;
  size_t derialize(ConfigMemType& buff, size_t offset);

private:
  Curve temperature_curve_;
  Fan *fan_ = nullptr;
  Temperature *sensor_ = nullptr;
  int8_t sensor_idx_ = -1;
  microsDelay update_delay_;
};
} // namespace LB
