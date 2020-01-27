#include "config.h"

#include <algorithm>
#include <vector>

#include <SPI.h>

namespace LB {
constexpr std::array<FanConfig, Constants::NUM_FANS> Config::fan_configs;
constexpr std::array<LedConfig, Constants::NUM_LEDS> Config::led_configs;
constexpr std::array<TemperatureConfig, Constants::NUM_TEMPERATURE_SENSORS>
    Config::temp_configs;
std::array<std::unique_ptr<MCP4822>, Constants::NUM_DACS> Config::dacs_;
std::array<uint8_t, Constants::NUM_DACS> Config::dac_ss_pins_;
std::array<uint8_t, Constants::NUM_DACS> Config::dac_indices_;
std::array<Fan, Constants::NUM_FANS> Config::fans;
std::array<LED, Constants::NUM_LEDS> Config::leds;
std::array<Temperature, Constants::NUM_TEMPERATURE_SENSORS>
    Config::temperature_sensors;
std::array<FanControl, Constants::NUM_FANS> Config::fan_controls;
std::array<LEDControl, Constants::NUM_LEDS> Config::led_controls;

void Config::pre_init_dac_(MCP4822 &dac) {
  dac.init();
  dac.turnOnChannelA();
  dac.turnOnChannelB();
  dac.setGainA(MCP4822::High);
  dac.setGainB(MCP4822::High);
}

void Config::init_dacs_() {
  uint8_t index = 0;

  for (const auto &fan_config : fan_configs) {
    if (std::find(dac_ss_pins_.begin(), dac_ss_pins_.end(),
                  fan_config.dac_ss_pin) == dac_ss_pins_.end()) {
      dac_ss_pins_[index++] = fan_config.dac_ss_pin;
    }
  }

  for (const auto &led_config : led_configs) {
    if (std::find(dac_ss_pins_.begin(), dac_ss_pins_.end(),
                  led_config.r_channel.dac_ss_pin) == dac_ss_pins_.end()) {
      dac_ss_pins_[index++] = led_config.r_channel.dac_ss_pin;
    }
    if (std::find(dac_ss_pins_.begin(), dac_ss_pins_.end(),
                  led_config.g_channel.dac_ss_pin) == dac_ss_pins_.end()) {
      dac_ss_pins_[index++] = led_config.g_channel.dac_ss_pin;
    }
    if (std::find(dac_ss_pins_.begin(), dac_ss_pins_.end(),
                  led_config.b_channel.dac_ss_pin) == dac_ss_pins_.end()) {
      dac_ss_pins_[index++] = led_config.b_channel.dac_ss_pin;
    }
  }

  for (uint i = 0; i < dac_ss_pins_.size(); i++) {
    dacs_[i] = std::make_unique<MCP4822>(dac_ss_pins_[i]);
    dac_indices_[i] = dac_ss_pins_[i];
    pre_init_dac_(*dacs_[i]);
  }
}

void Config::init_fans_() {
  for (uint i = 0; i < fan_configs.size(); i++) {
    const auto &fan_config = fan_configs[i];
    const auto &itr = std::find(dac_ss_pins_.begin(), dac_ss_pins_.end(),
                                fan_config.dac_ss_pin);
    const auto arr_index = std::distance(dac_ss_pins_.begin(), itr);

    auto &fan = fans[i];

    if (fan_config.dac_channel == 0) {
      fan.init(
          i, fan_config.tach_pin, fan_config.pwm_pin,
          [arr_index](uint16_t value) {
            auto &dac = *dacs_[arr_index];
            dac.setVoltageA(value);
            dac.updateDAC();
          },
          false);
    } else {
      fan.init(
          i, fan_config.tach_pin, fan_config.pwm_pin,
          [arr_index](uint16_t value) {
            auto &dac = *dacs_[arr_index];
            dac.setVoltageB(value);
            dac.updateDAC();
          },
          false);
    }

    fan.begin();
  }
}

void Config::init_leds_() {
  for (uint i = 0; i < led_configs.size(); i++) {
    const auto &led_config = led_configs[i];

    std::array<SetDacFunc, Constants::NUM_LED_CHANNELS> set_dac_funcs;

    for (uint j = 0; j < Constants::NUM_LED_CHANNELS; j++) {
      const auto *channel_config =
          ((LedChannelConfig *)(((char *)&led_config) +
                                sizeof(LedChannelConfig) * j));
      const auto &itr = std::find(dac_ss_pins_.begin(), dac_ss_pins_.end(),
                                  channel_config->dac_ss_pin);
      const auto arr_index = std::distance(dac_ss_pins_.begin(), itr);

      if (channel_config->dac_channel == 0) {
        set_dac_funcs[j] = std::move([arr_index](uint16_t value) {
          auto &dac = *dacs_[arr_index];
          dac.setVoltageA(value);
          dac.updateDAC();
        });
      } else {
        set_dac_funcs[j] = std::move([arr_index](uint16_t value) {
          auto &dac = *dacs_[arr_index];
          dac.setVoltageB(value);
          dac.updateDAC();
        });
      }
    }

    auto &led = leds[i];
    led.init(std::move(set_dac_funcs[0]), std::move(set_dac_funcs[1]),
             std::move(set_dac_funcs[2]));
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

    fan_control.init(fan, temperature_sensor);
  }
}

void Config::init_led_control_() {
  for (uint i = 0; i < leds.size(); i++) {
    auto &led_control = led_controls[i];
    auto &led = leds[i];

    led_control.init(led);
    // led_control.set_temperature_sensor(temperature_sensors[0]);
  }
}

void Config::init() {
  fan_pre_init();
  temperature_pre_init();

  SPI.setMOSI(MOSI_PIN);
  SPI.setSCK(SCK_PIN);

  init_dacs_();
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
