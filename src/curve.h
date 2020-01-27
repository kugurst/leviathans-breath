#pragma once

#include <array>

#include "constants.h"

namespace LB {
const uint8_t LO = 0;
const uint8_t HI = 1;

class Curve {
public:
  std::array<Point, Constants::POINTS_PER_CURVE> &get_points() {
    return points_;
  };
  const std::array<Point, Constants::POINTS_PER_CURVE> &
  get_points_const() const {
    return points_;
  };
  uint16_t get_valid_points() const { return valid_points_; };
  void set_valid_points(uint16_t points) { valid_points_ = points; };
  std::tuple<Point, Point> search(const Dimension &x) const;

private:
  std::array<Point, Constants::POINTS_PER_CURVE> points_ = {};
  uint16_t valid_points_ = 0;
};
} // namespace LB
