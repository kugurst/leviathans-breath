#pragma once

#include <array>

#include <SD.h>

#include "constants.hpp"

namespace LB {
const char *CONFIG_FILE_NAME = "config.dat";
const int CHIP_SELECT = BUILTIN_SDCARD;
const uint8_t MAX_FILE_RETRY = 3;
// curve (points + valid points)
const size_t SERIALIZED_FAN_CONTROL_BYTES =
    sizeof(Point) * Constants::POINTS_PER_CURVE + sizeof(uint16_t);
// curves + time_controlled + speed
const size_t SERIALIZED_LED_CONTROL_BYTES =
    (sizeof(Point) * Constants::POINTS_PER_CURVE + sizeof(uint16_t)) *
    Constants::NUM_LED_CHANNELS + sizeof(bool) + sizeof(CalculatorDimension);

class FileManager {
public:
  static void init();
  static void save_config();
  static bool load_config();
  static void load_defaults();

private:
  static File config_file_;
  static bool sd_initialized_;
  static ConfigMemType config_mem_map_;
  static uint8_t file_retry_count_;
};
} // namespace LB
