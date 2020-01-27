#include "temperture.h"

#include <algorithm>
#include <cmath>

namespace LB {
void temperature_pre_init() {
  analogReadResolution(TEMPERATURE_RESOLUTION);
  analogReadAveraging(8);
}

void Temperature::init(uint8_t channel, uint8_t sensor_pin,
                       TemperatureProbeType probe_type) {
  sensor_pin_ = sensor_pin;
  channel_ = channel;
  probe_type_ = probe_type;
  sample_delay_.start(TEMPERATURE_SAMPLE_DELAY_US);
}

void Temperature::loop() {
  if (!sample_delay_.justFinished()) {
    return;
  } else {
    sample_delay_.repeat();
  }

  uint16_t sample = analogRead(sensor_pin_);
  temperature_samples_.push_back(sample);
  num_valid_samples = min(num_valid_samples + 1, NUM_TEMPERATURE_SAMPLES);
}

float Temperature::read_temperature() {
  auto itr = 0;

  float average_reading = 0.0f;
  for (auto sample : temperature_samples_.get_buffer()) {
    average_reading += sample;
    itr++;
    if (itr == num_valid_samples) {
      break;
    }
  }
  average_reading /= num_valid_samples;

  const float scaled_voltage = average_reading / (float)MAX_TEMPERATURE_READING;
  const float resistance =
      -(SENSE_RESISTOR_VALUE * scaled_voltage) / (scaled_voltage - 1.0f);

  const auto &sensor_constants = get_temperature_constants(probe_type_);
  const float temperature =
      1.0f / (sensor_constants.k1 + sensor_constants.k2 * log(resistance) +
              sensor_constants.k3 * pow(log(resistance), 3.0f)) -
      KELVIN_TO_CELCIUS;

  return temperature;
}
} // namespace LB
