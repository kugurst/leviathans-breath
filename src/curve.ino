#include "curve.h"

namespace LB {
std::tuple<Point, Point> Curve::search(const Dimension &x) const {
  if (x < std::get<X>(points_[0])) {
    return std::make_tuple(points_[0], points_[0]);
  }
  if (x > std::get<X>(points_[valid_points_ - 1])) {
    return std::make_tuple(points_[valid_points_ - 1],
                           points_[valid_points_ - 1]);
  }

  auto lo = 0;
  auto hi = valid_points_ - 1;

  while (lo <= hi) {
    auto mid = (hi + lo) / 2;

    if (x < std::get<X>(points_[mid])) {
      hi = mid - 1;
    } else if (x > std::get<X>(points_[mid])) {
      lo = mid + 1;
    } else {
      return std::make_tuple(points_[mid], points_[mid]);
    }
  }
  // lo == hi + 1
  // Serial.print("MIN: (");
  // Serial.print((float)std::get<X>(points_[hi]));
  // Serial.print(", ");
  // Serial.print((float)std::get<Y>(points_[hi]));
  // Serial.print(") | MAX: (");
  // Serial.print((float)std::get<X>(points_[lo]));
  // Serial.print(", ");
  // Serial.print((float)std::get<Y>(points_[lo]));
  // Serial.println(")");
  return std::make_tuple(points_[hi], points_[lo]);
}
} // namespace LB
