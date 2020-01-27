#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cstdint>

#include <array>
#include <tuple>

#include "utils.h"

#include <FixedPoints.h>
#include <FixedPointsCommon.h>

namespace LB {
using Dimension = FixedPoints::UFixed<8, 8>;
using CalculatorDimension = FixedPoints::UFixed<10, 22>;
using Point = std::tuple<Dimension, Dimension>;

const uint8_t X = 0;
const uint8_t Y = 1;

struct TemperatureSensorConstants {
  const float k1;
  const float k2;
  const float k3;
};

class Constants {
public:
  static const uint32_t CONFIG_FILE_SIZE_BYTES = 1 << 15;
  static const uint32_t NUM_FANS = 8;
  static const uint32_t NUM_DACS = 7;
  static const uint32_t NUM_LEDS = 2;
  static const uint32_t NUM_LED_CHANNELS = 3;
  static const uint32_t NUM_TEMPERATURE_SENSORS = 2;

  static const uint32_t NUM_CURVES = NUM_FANS + NUM_LEDS * NUM_LED_CHANNELS;
  static const uint32_t CONFIG_SPACE_BYTES = 512;
  static const uint32_t CURVE_BYTES =
      (CONFIG_FILE_SIZE_BYTES - CONFIG_SPACE_BYTES) / NUM_CURVES;

  static const uint32_t POINTS_PER_CURVE = CURVE_BYTES / sizeof(Point);

  static constexpr TemperatureSensorConstants BARROW_CONSTANTS = {
      -1.80284535e-04, 4.37755038e-04, -6.24132123e-07};
};

using ConfigMemType = std::array<byte, Constants::CONFIG_FILE_SIZE_BYTES>;
} // namespace LB

#endif
