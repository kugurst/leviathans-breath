#ifndef __FAN_H__
#define __FAN_H__

#include <array>
#include <cstdint>
#include <functional>
#include <utility>

#include "utils.h"

#ifdef CUSTOM_TACH
#include <microsDelay.h>
#else
#include <FreqMeasureMulti.h>
#endif

namespace LB {
const float MAX_VOLTAGE = 12.0f;
const float MAX_PWM = 100.0f;
const uint16_t PWM_HZ = 25000;
const uint8_t PWM_RESOLUTION = 12;
const uint8_t READ_SAMPLE_COUNT = 2;
const uint8_t READ_SAMPLE_WINDOW_LENGTH = 5;

void fan_pre_init();

enum struct READING_STATE { HI, LO, X };

class Fan {
public:
  void init(uint8_t channel, uint8_t tach_pin, uint8_t pwm_pin,
            SetDacFunc set_voltage_function, bool pwm_controlled);

  void begin();
  void set_voltage(float voltage);
  const inline float get_voltage() { return voltage_; };
  void set_pwm(float pwm);
  const inline float get_pwm() { return pwm_; };
  const inline bool is_pwm_controlled() { return pwm_controlled_; };
  void loop();
  const float read_rpm();

private:
  SetDacFunc set_dac_ = nullptr;
  uint8_t pwm_pin_ = 0;
  uint8_t tach_pin_ = 0;
  uint8_t channel_ = 0;

  float voltage_ = MAX_VOLTAGE;
  float pwm_ = MAX_PWM;
  bool pwm_controlled_ = false;

#ifndef CUSTOM_TACH
  FreqMeasureMulti rpm_reader_;
  uint64_t freq_reading_ = 0;
  uint32_t freq_reading_count_ = 0;
#else
  float rpm_ = 0;
  std::array<uint32_t, READ_SAMPLE_COUNT> lo_time_ = {};
  uint8_t lo_count_ = 0;
  std::array<uint32_t, READ_SAMPLE_COUNT> hi_time_ = {};
  uint8_t hi_count_ = 0;
  std::array<uint32_t, READ_SAMPLE_WINDOW_LENGTH> read_samples_ = {};
  uint8_t read_samples_head_ = 0;
  uint8_t read_samples_valid_ = 0;
  READING_STATE read_state_ = READING_STATE::X;
  bool rpm_changed_since_last_read = false;
  microsDelay sample_delay;
#endif
};
} // namespace LB

#endif
