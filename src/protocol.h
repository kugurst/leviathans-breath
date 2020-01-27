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

namespace LB {
const uint8_t HID_BUF_SIZE = 64;
const uint8_t HID_TIMEOUT = 100;
const uint16_t CHECK_DELAY_US = 2000;

// All commands are issued by the master
enum Command : uint8_t {
  SEND_CURVE,
  RECEIVE_CURVE,
  SEND_ALL_RPM,
  SEND_ALL_TEMPERATURE,
  GET_ALL_FAN_PARAMETERS,
  SET_ALL_FAN_PARAMETERS,
  GET_ALL_LED_PARAMETERS,
  SET_ALL_LED_PARAMETERS,
};

enum CurveType : uint8_t {
  FAN_CURVE, LED_CURVE
};

enum CurveChannel : uint8_t {
  R_CHAN, G_CHAN, B_CHAN
};

struct CurveCommandParameters {
  CurveType curve_type;
  CurveChannel curve_channel;
  uint8_t curve_idx;
};

struct FanParameters {
  bool pwm_controlled;
  float pwm;
  float voltage;
};

struct LEDParameters {
  bool time_controlled;
  float speed_multiplier;
};

class Protocol {
public:
  static void init();
  static void loop();

private:
  static void print_command_(Command command);
  static void process_command_(Command command);
  static void send_curve_(Curve& curve);
  static bool send_all_rpm_();
  static bool send_all_temperatures_();
  static bool get_all_fan_parameters_();
  static bool send_start_packet_(Command command, uint32_t num_packets);
  static microsDelay check_delay_;
  static std::array<uint8_t, HID_BUF_SIZE> rx_hid_buf_;
  static std::array<uint8_t, HID_BUF_SIZE> tx_hid_buf_;
};
} // namespace LB
