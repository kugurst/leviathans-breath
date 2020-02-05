#include "file_manager.h"

#include <cstring>

#include "config.h"
#include "utils.h"

namespace LB {
File FileManager::config_file_;
bool FileManager::sd_initialized_ = false;
std::array<byte, Constants::CONFIG_FILE_SIZE_BYTES>
    FileManager::config_mem_map_;
uint8_t FileManager::file_retry_count_ = 0;

void FileManager::init() {
  config_mem_map_.fill(0);
  if (!SD.begin(CHIP_SELECT)) {
#ifndef DISABLE_SERIAL
    Serial.println(F("SD initialization failed!"));
#endif
    sd_initialized_ = false;
  } else {
    sd_initialized_ = true;
  }
}

void FileManager::save_config() {
  if (!sd_initialized_) {
    return;
  }

  size_t offset = 0;

  for (const auto &fan_control : Config::fan_controls) {
    offset += fan_control.serialize(config_mem_map_, offset);
  }
  for (const auto &led_control : Config::led_controls) {
    offset += led_control.serialize(config_mem_map_, offset);
  }

  while (file_retry_count_ < MAX_FILE_RETRY) {
    config_file_ =
        std::move(SD.open(CONFIG_FILE_NAME, O_CREAT | O_TRUNC | O_WRITE));

    if (config_file_) {
      config_file_.seek(0);
      config_file_.write(config_mem_map_.data(), config_mem_map_.size());
      config_file_.close();
      break;
    } else {
      // if the file didn't open, print an error:
#ifndef DISABLE_SERIAL
      Serial.print(F("error saving "));
      Serial.println(CONFIG_FILE_NAME);
#endif
    }
#ifndef DISABLE_SERIAL
    Serial.println(F("retrying save"));
#endif
    file_retry_count_++;
  }
  file_retry_count_ = 0;
}

bool FileManager::load_config() {
  if (!sd_initialized_) {
    load_defaults();
    return false;
  }

  bool file_read = false;
  while (file_retry_count_ < MAX_FILE_RETRY) {
    config_file_ = std::move(SD.open(CONFIG_FILE_NAME, O_READ));
    if (config_file_) {
      config_file_.read(config_mem_map_.data(), config_mem_map_.size());
      config_file_.close();
      file_read = true;
      break;
    } else {
#ifndef DISABLE_SERIAL
      // if the file didn't open, print an error:
      Serial.print(F("error opening "));
      Serial.println(CONFIG_FILE_NAME);
#endif
    }
#ifndef DISABLE_SERIAL
    Serial.println(F("retrying read"));
#endif
    file_retry_count_++;
  }
  file_retry_count_ = 0;
  if (!file_read) {
    load_defaults();
#ifndef DISABLE_SERIAL
    Serial.println(F("Failed to read file! Loading defaults"));
#endif
    return false;
  }

  size_t offset = 0;

  for (auto &fan_control : Config::fan_controls) {
    offset += fan_control.derialize(config_mem_map_, offset);
    const auto sensor_idx = fan_control.get_temperature_sensor();
    fan_control.set_temperature_sensor(
        Config::get_temperature_sensor(sensor_idx), sensor_idx);
  }
  for (auto &led_control : Config::led_controls) {
    offset += led_control.derialize(config_mem_map_, offset);
    const auto sensor_idx = led_control.get_temperature_sensor();
    if (sensor_idx >= 0 && !led_control.is_time_controlled()) {
      led_control.set_temperature_sensor(
          Config::get_temperature_sensor(sensor_idx), sensor_idx);
    }
  }

  return true;
}

void FileManager::load_defaults() {
  for (auto &fan_control : Config::fan_controls) {
    auto &curve = fan_control.get_curve();
    curve.get_points()[0] = std::move(std::make_tuple(0.0f, 0.0f));
    curve.get_points()[1] = std::move(std::make_tuple(20.0f, 40.0f));
    curve.get_points()[2] = std::move(std::make_tuple(30.0f, 80.0f));
    curve.get_points()[3] = std::move(std::make_tuple(40.0f, 100.0f));
    curve.set_valid_points(4);
  }

  for (auto &led_control : Config::led_controls) {
    auto curve_counter = 0;
    for (auto &curve : led_control.get_curves()) {
      auto &points = curve.get_points();

      points[0] = std::move(std::make_tuple(0.0f, 0.0f));
      points[1] = std::move(
          std::make_tuple((float)MAX_TIME_STEP / 2.0f, MAX_BRIGHTNESS));
      points[2] = std::move(std::make_tuple(MAX_TIME_STEP.getInteger(), 0.0f));
      curve.set_valid_points(3);

      curve_counter++;
    }
  }
}
} // namespace LB
