#include "fan_control.h"

namespace LB {
void FanControl::init(Fan &fan, Temperature &sensor) {
  fan_ = &fan;
  sensor_ = &sensor;
  update_delay_.start(UPDATE_PERIOD_US);
}

void FanControl::loop() {
  if (!update_delay_.justFinished()) {
    return;
  } else {
    update_delay_.repeat();
  }

  const Dimension temperature = sensor_->read_temperature();
  const auto bounds = temperature_curve_.search(temperature);
  const auto range = (CalculatorDimension)std::get<X>(std::get<HI>(bounds)) -
                     (CalculatorDimension)std::get<X>(std::get<LO>(bounds));

  CalculatorDimension ratio;
  if (range == 0) {
    ratio = 1; // exact match
  } else {
    ratio = ((CalculatorDimension)temperature -
             (CalculatorDimension)std::get<X>(std::get<LO>(bounds))) /
            range;
  }

  const auto interpolated =
      (float)ratio * ((float)std::get<Y>(std::get<HI>(bounds)) -
                      (float)std::get<Y>(std::get<LO>(bounds))) +
      (float)std::get<Y>(std::get<LO>(bounds));

  if (fan_->is_pwm_controlled()) {
    fan_->set_pwm(interpolated);
  } else {
    fan_->set_voltage((interpolated / 100.0f) * MAX_VOLTAGE);
  }
}

size_t FanControl::serialize(ConfigMemType &buff, size_t offset) const {
  size_t original_offset = offset;

  const auto &points = temperature_curve_.get_points_const();
  const auto valid_points = temperature_curve_.get_valid_points();

  memcpy((void *)(((uint8_t *)buff.data()) + offset), (void *)points.data(),
         sizeof(Point) * points.size());
  offset += sizeof(Point) * points.size();
  memcpy((void *)(((uint8_t *)buff.data()) + offset), (void *)&valid_points,
         sizeof(uint16_t));
  offset += sizeof(uint16_t);

  return offset - original_offset;
}

size_t FanControl::derialize(ConfigMemType &buff, size_t offset) {
  size_t original_offset = offset;

  auto &points = temperature_curve_.get_points();
  uint16_t valid_points;

  memcpy((void *)points.data(), (void *)(((uint8_t *)buff.data()) + offset),
         sizeof(Point) * points.size());
  offset += sizeof(Point) * points.size();
  memcpy((void *)&valid_points, (void *)(((uint8_t *)buff.data()) + offset),
         sizeof(uint16_t));
  offset += sizeof(uint16_t);

  temperature_curve_.set_valid_points(valid_points);

  return offset - original_offset;
}
} // namespace LB
