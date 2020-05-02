#include "config.h"

#include <algorithm>
#include <vector>

namespace LB {
constexpr std::array<FanConfig, Constants::NUM_FANS> Config::fan_configs;
constexpr std::array<LedConfig, Constants::NUM_LEDS> Config::led_configs;
constexpr std::array<TemperatureConfig, Constants::NUM_TEMPERATURE_SENSORS>
    Config::temp_configs;
std::array<Fan, Constants::NUM_FANS> Config::fans;
std::array<LED, Constants::NUM_LEDS> Config::leds;
std::array<Temperature, Constants::NUM_TEMPERATURE_SENSORS>
    Config::temperature_sensors;
std::array<FanControl, Constants::NUM_FANS> Config::fan_controls;
std::array<LEDControl, Constants::NUM_LEDS> Config::led_controls;

void Config::init_pwm_() { analogWriteResolution(PWM_RESOLUTION); }

void Config::init_fans_() {
  for (uint i = 0; i < fan_configs.size(); i++) {
    const auto &fan_config = fan_configs[i];
    auto &fan = fans[i];

    fan.init(i, fan_config.tach_pin, fan_config.pwm_pin, fan_config.gd_pin,
             false);
    fan.begin();
  }
}

void Config::init_leds_() {
  for (uint i = 0; i < led_configs.size(); i++) {
    const auto &led_config = led_configs[i];
    auto &led = leds[i];

    led.init(led_config.r_channel.gd_pin, led_config.g_channel.gd_pin,
             led_config.b_channel.gd_pin);
  }
}

void Config::init_temperature_sensors_() {
  for (uint i = 0; i < temp_configs.size(); i++) {
    const auto &temp_config = temp_configs[i];
    auto &temperature_sensor = temperature_sensors[i];

    temperature_sensor.init(i, temp_config.analog_pin, temp_config.probe_type);
  }
}

void Config::init_fan_control_() {
  for (uint i = 0; i < fans.size(); i++) {
    auto &fan_control = fan_controls[i];
    auto &fan = fans[i];
    auto &temperature_sensor = temperature_sensors[0];

    fan_control.init(fan, temperature_sensor, 0);
  }
}

void Config::init_led_control_() {
  for (uint i = 0; i < leds.size(); i++) {
    auto &led_control = led_controls[i];
    auto &led = leds[i];

    led_control.init(led);
  }
}

void Config::init() {
  temperature_pre_init();

  init_pwm_();
  init_fans_();
  init_leds_();
  init_temperature_sensors_();
  init_fan_control_();
  init_led_control_();
}

void Config::loop_fans() {
  for (auto &fan : fans) {
    fan.loop();
  }
}

void Config::loop_temperature_sensors() {
  for (auto &temperature_sensor : temperature_sensors) {
    temperature_sensor.loop();
  }
}

void Config::loop_fan_controls() {
  for (auto &fan_control : fan_controls) {
    fan_control.loop();
  }
}

void Config::loop_led_controls() {
  for (auto &led_control : led_controls) {
    led_control.loop();
  }
}

void Config::loop_all() {
  loop_fans();
  loop_temperature_sensors();

  loop_fan_controls();
  loop_led_controls();
}
} // namespace LB
