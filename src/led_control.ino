#include "led_contol.h"

#include "math.h"

namespace LB {
void LEDControl::init(LED &led) {
  led_ = &led;
  update_delay_.start(LED_UPDATE_PERIOD_US);
}

void LEDControl::set_temperature_sensor(Temperature &sensor) {
  sensor_ = &sensor;
  time_controlled_ = false;
  current_time_step_ = 0;
}

void LEDControl::loop() {
  if (!update_delay_.justFinished()) {
    return;
  } else {
    update_delay_.repeat();
  }

  std::array<float, Constants::NUM_LED_CHANNELS> brightness_levels = {};
  for (uint i = 0; i < curves_.size(); i++) {
    const auto &curve = curves_[i];

    std::tuple<Point, Point> bounds;
    Dimension range;
    CalculatorDimension ratio;

    if (time_controlled_) {
      const auto time_step = current_time_step_;
      bounds = curve.search((Dimension)time_step);
      range =
          std::get<X>(std::get<HI>(bounds)) - std::get<X>(std::get<LO>(bounds));

      if (range == 0) {
        ratio = 1; // exact match
      } else {
        ratio = (time_step -
                 (CalculatorDimension)std::get<X>(std::get<LO>(bounds))) /
                (CalculatorDimension)range;
      }
    } else {
      const Dimension temperature = (Dimension)sensor_->read_temperature();
      bounds = curve.search(temperature);
      range =
          std::get<X>(std::get<HI>(bounds)) - std::get<X>(std::get<LO>(bounds));

      if (range == 0) {
        ratio = 1; // exact match
      } else {
        ratio = (((CalculatorDimension)temperature) -
                 (CalculatorDimension)std::get<X>(std::get<LO>(bounds))) /
                (CalculatorDimension)range;
      }
    }

    const auto interpolated =
        ((float)ratio) * ((float)std::get<Y>(std::get<HI>(bounds)) -
                          (float)std::get<Y>(std::get<LO>(bounds))) +
        (float)std::get<Y>(std::get<LO>(bounds));

    brightness_levels[i] = interpolated;
  }

  led_->set_r_brightness(brightness_levels[0]);
  led_->set_g_brightness(brightness_levels[1]);
  led_->set_b_brightness(brightness_levels[2]);

  if (time_controlled_) {
    if (current_time_step_ == MAX_TIME_STEP) {
      current_time_step_ = 0;
    } else {
      current_time_step_ = min(
          MAX_TIME_STEP, current_time_step_ + (TIME_STEP_INCREMENT * speed_));
    }
  }
}

size_t LEDControl::serialize(ConfigMemType &buff, size_t offset) const {
  size_t original_offset = offset;

  for (const auto &curve : curves_) {
    const auto &points = curve.get_points_const();
    const auto valid_points = curve.get_valid_points();

    memcpy((void *)(((uint8_t *)buff.data()) + offset), (void *)points.data(),
           sizeof(Point) * points.size());
    offset += sizeof(Point) * points.size();
    memcpy((void *)(((uint8_t *)buff.data()) + offset), (void *)&valid_points,
           sizeof(uint16_t));
    offset += sizeof(uint16_t);
  }

  memcpy((void *)(((uint8_t *)buff.data()) + offset), (void *)&time_controlled_,
         sizeof(bool));
  offset += sizeof(bool);

  memcpy((void *)(((uint8_t *)buff.data()) + offset), (void *)&speed_,
         sizeof(CalculatorDimension));
  offset += sizeof(CalculatorDimension);

  return offset - original_offset;
}

size_t LEDControl::derialize(ConfigMemType &buff, size_t offset) {
  size_t original_offset = offset;

  for (auto &curve : curves_) {
    auto &points = curve.get_points();
    uint16_t valid_points;

    memcpy((void *)points.data(), (void *)(((uint8_t *)buff.data()) + offset),
           sizeof(Point) * points.size());
    offset += sizeof(Point) * points.size();
    memcpy((void *)&valid_points, (void *)(((uint8_t *)buff.data()) + offset),
           sizeof(uint16_t));
    offset += sizeof(uint16_t);

    curve.set_valid_points(valid_points);
  }

  memcpy((void *)&time_controlled_, (void *)(((uint8_t *)buff.data()) + offset),
         sizeof(bool));
  offset += sizeof(bool);

  memcpy((void *)&speed_, (void *)(((uint8_t *)buff.data()) + offset),
         sizeof(CalculatorDimension));
  offset += sizeof(CalculatorDimension);

  return offset - original_offset;
}
} // namespace LB
