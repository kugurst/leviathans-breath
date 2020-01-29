#pragma once

#define VENDOR_ID 0x16C0
#define PRODUCT_ID 0x0480
#define RAWHID_USAGE_PAGE 0xFFAB // recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE 0x0200      // recommended: 0x0100 to 0xFFFF

#define RAWHID_TX_SIZE 64    // transmit packet size
#define RAWHID_TX_INTERVAL 2 // max # of ms between transmit packets
#define RAWHID_RX_SIZE 64    // receive packet size
#define RAWHID_RX_INTERVAL 8 // max # of ms between receive packets

#include <array>

#include <microsDelay.h>
#include <usb_rawhid.h>

#include "constants.h"
#include "curve.h"
#include "file_manager.h"

namespace LB {
const uint8_t HID_BUF_SIZE = 64;
const uint8_t HID_TIMEOUT = 100;
const uint16_t CHECK_DELAY_US = 2000;

// All commands are issued by the master
enum Command : uint8_t {
  GET_CURVE,
  SET_CURVE,
  GET_ALL_RPM,
  GET_ALL_TEMPERATURE,
  GET_ALL_FAN_PARAMETERS,
  SET_FAN_PARAMETERS,
  GET_ALL_LED_PARAMETERS,
  SET_LED_PARAMETERS,
};

enum CurveType : uint8_t {
  FAN_CURVE, LED_CURVE
};

enum CurveChannel : uint8_t {
  R_CHAN, G_CHAN, B_CHAN
};

struct CurveCommandParameters {
  uint8_t channel;
  CurveType curve_type;
  CurveChannel rgb_channel;
  uint16_t curve_length;
};

struct FanParameters {
  uint8_t channel;
  bool pwm_controlled;
  float pwm;
  float voltage;
};

struct LEDParameters {
  uint8_t channel;
  bool time_controlled;
  float speed_multiplier;
  float r_brightness;
  float g_brightness;
  float b_brightness;
};

class Protocol {
public:
  static void init();
  static void loop();

private:
  static void print_command_(Command command);
  static void process_command_(Command command, std::array<uint8_t, HID_BUF_SIZE>& extra_data, uint8_t extra_data_offset);
  static bool set_curve_(CurveCommandParameters curve_command);
  static bool get_curve_(CurveCommandParameters curve_command);
  static bool send_all_rpm_();
  static bool send_all_temperatures_();
  static bool get_all_fan_parameters_();
  static bool set_fan_parameters_(FanParameters fan_command);
  static bool get_all_led_parameters_();
  static bool set_led_parameters_(LEDParameters led_command);
  static bool send_start_packet_(Command command, uint32_t num_packets);
  static microsDelay check_delay_;
  static std::array<uint8_t, HID_BUF_SIZE> rx_hid_buf_;
  static std::array<uint8_t, HID_BUF_SIZE> tx_hid_buf_;
};
} // namespace LB
