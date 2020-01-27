#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <cstdint>

#include <microsDelay.h>

#include "circular_array.h"
#include "constants.h"

namespace LB {
const uint8_t TEMPERATURE_RESOLUTION = 12;
const uint16_t MAX_TEMPERATURE_READING = 4095;
const uint16_t TEMPERATURE_SAMPLE_DELAY_US = 1000;
const uint8_t NUM_TEMPERATURE_SAMPLES = 10;
const uint16_t SENSE_RESISTOR_VALUE = 10000;
const float KELVIN_TO_CELCIUS = 273.15;

void temperature_pre_init();

enum TemperatureProbeType { BARROW };

static inline const TemperatureSensorConstants &
get_temperature_constants(TemperatureProbeType type) {
  switch (type) {
  case TemperatureProbeType::BARROW:
    return Constants::BARROW_CONSTANTS;
  default:
    return Constants::BARROW_CONSTANTS;
  }
}

class Temperature {
public:
  void init(uint8_t channel, uint8_t sensor_pin,
            TemperatureProbeType probe_type);
  void loop();
  float read_temperature();

private:
  CircularArray<uint16_t, NUM_TEMPERATURE_SAMPLES> temperature_samples_;
  uint8_t num_valid_samples = 0;
  uint8_t sensor_pin_;
  uint8_t channel_;
  microsDelay sample_delay_;
  TemperatureProbeType probe_type_;
};
} // namespace LB

#endif
