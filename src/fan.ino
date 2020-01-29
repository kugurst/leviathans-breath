#include "fan.h"

#include <cstdint>

#include <array>
#include <cmath>
#include <cstring>

namespace LB {
static const uint16_t MAX_DAC_VALUE = 4095;
static const uint16_t MAX_PWM_RAW = 4095;
#ifndef CUSTOM_TACH
static const uint8_t MAX_RPM_COUNT = 100;
#else
static const uint8_t TACH_SAMPLE_DELAY_US = 1;
#endif

void fan_pre_init() { analogWriteResolution(PWM_RESOLUTION); }

void Fan::init(uint8_t channel, uint8_t tach_pin, uint8_t pwm_pin,
               SetDacFunc set_voltage_function, bool pwm_controlled) {
  set_dac_ = set_voltage_function;
  pwm_pin_ = pwm_pin;
  tach_pin_ = tach_pin;
  channel_ = channel;
  voltage_ = MAX_VOLTAGE;
  pwm_ = MAX_PWM;
  pwm_controlled_ = pwm_controlled;
}

void Fan::begin() {
  pinMode(pwm_pin_, OUTPUT);
  analogWriteFrequency(pwm_pin_, PWM_HZ);
  analogWrite(pwm_pin_, pwm_ * MAX_PWM_RAW);

#ifdef CUSTOM_TACH
  pinMode(tach_pin_, INPUT);
  sample_delay.start(TACH_SAMPLE_DELAY_US);
#else
  rpm_reader_.begin(tach_pin_);
#endif

  set_dac_(MAX_DAC_VALUE * voltage_);
}

void Fan::set_voltage(float voltage) {
  voltage_ = fmin(MAX_VOLTAGE, fmax(0, voltage));
  set_dac_(voltage_ / MAX_VOLTAGE * MAX_DAC_VALUE);

  if (pwm_controlled_) {
    pwm_controlled_ = false;
    analogWrite(pwm_pin_, MAX_PWM_RAW);
  }
}

void Fan::set_pwm(float pwm) {
  pwm_ = fmin(MAX_PWM, fmax(0, pwm));
  analogWrite(pwm_pin_, pwm_ / MAX_PWM * MAX_PWM_RAW);

  if (!pwm_controlled_) {
    pwm_controlled_ = true;
    set_dac_(MAX_DAC_VALUE);
  }
}

void Fan::set_pwm_controlled(bool pwm_controlled) {
  pwm_controlled_ = pwm_controlled;
  if (pwm_controlled_) {
    set_dac_(MAX_DAC_VALUE);
  }
}

void Fan::loop() {
#ifndef CUSTOM_TACH
  if (rpm_reader_.available()) {
    if (freq_reading_count_ >= MAX_RPM_COUNT) {
      freq_reading_ = 0;
      freq_reading_count_ = 0;
    }
    freq_reading_ += rpm_reader_.read();
    freq_reading_count_++;
  }
#else
  if (!sample_delay.justFinished()) {
    return;
  } else {
    sample_delay.repeat();
  }

  // we missed a transition somewhere, somehow. Try again
  if (lo_count_ >= READ_SAMPLE_COUNT || hi_count_ >= READ_SAMPLE_COUNT) {
    lo_count_ = 0;
    hi_count_ = 0;
  }

  if (analogRead(tach_pin_) > 750 && read_state_ != READING_STATE::HI) {
    hi_time_[hi_count_++] = micros();
    read_state_ = READING_STATE::HI;
  } else if (analogRead(tach_pin_) < 100 && read_state_ != READING_STATE::LO) {
    lo_time_[lo_count_++] = micros();
    read_state_ = READING_STATE::LO;
  }

  if (lo_count_ >= 1 && hi_count_ >= 1) {
    uint32_t ellapsed = 0;
    bool ellapsed_valid = true;

    if (lo_count_ + 1 == hi_count_) {
      ellapsed = hi_time_[hi_count_ - 1] - lo_time_[lo_count_ - 1];
    } else if (hi_count_ + 1 == lo_count_) {
      ellapsed = lo_time_[lo_count_ - 1] - hi_time_[hi_count_ - 1];
    } else {
      ellapsed_valid = false;
    }

    if (ellapsed_valid) {
      read_samples_[read_samples_head_] = ellapsed;
      read_samples_head_ = (read_samples_head_ + 1) % READ_SAMPLE_WINDOW_LENGTH;
      read_samples_valid_ =
          min(READ_SAMPLE_WINDOW_LENGTH, read_samples_valid_ + 1);
      rpm_changed_since_last_read = true;
    }
  }
#endif
}

const float Fan::read_rpm() {
#ifdef CUSTOM_TACH
  uint8_t samples_averaged = 0;
  uint32_t ellapsed_total = 0;

  if (!rpm_changed_since_last_read) {
    return 0;
  }

  rpm_changed_since_last_read = false;

  while (samples_averaged < read_samples_valid_) {
    ellapsed_total += read_samples_[(read_samples_valid_ + samples_averaged) %
                                    READ_SAMPLE_WINDOW_LENGTH];
    samples_averaged++;
  }

  if (samples_averaged == 0) {
    return 0;
  }

  const float average_period = ellapsed_total / samples_averaged;
  const float frequency = (1.0f / (average_period * 1e-6f)) / 2.0f;
  return frequency * 60.0f / 2.0f;
#else
  return rpm_reader_.countToFrequency(freq_reading_ /
                                      (float)freq_reading_count_) *
         60 / 2;
#endif
}
} // namespace LB
