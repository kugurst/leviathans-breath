#include "driver.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(OS_LINUX) || defined(OS_MACOSX)
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(OS_WINDOWS)
#include <conio.h>
#endif

#include "hid.h"

#include <iostream>
#include <tuple>
#include <vector>

#include "nlohmann/json.hpp"

static char get_keystroke(void);

namespace LB {
static void print_bytes_received(int bytes_received,
                                 std::array<uint8_t, HID_BUF_SIZE> &buf) {
  printf("\nrecv %d bytes:\n", bytes_received);
  for (auto i = 0; i < bytes_received; i++) {
    printf("%02X ", buf[i] & 255);
    if (i % 16 == 15 && i < bytes_received - 1)
      printf("\n");
  }
  printf("\n");
}

bool Driver::connect() {
  if (driver_handle_ != DEFAULT_DRIVER_HANDLE) {
    return true;
  }

  // C-based example is 16C0:0480:FFAB:0200
  auto num_opened = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
  if (num_opened <= 0) {
    // Arduino-based example is 16C0:0486:FFAB:0200
    num_opened = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
    if (num_opened <= 0) {
      printf("no rawhid device found\n");
      return false;
    }
  }

  driver_handle_ = 0;

  printf("found rawhid device\n");
  return true;
}

bool Driver::disconnect() {
  if (driver_handle_ == DEFAULT_DRIVER_HANDLE) {
    return true;
  }

  rawhid_close(driver_handle_);
  driver_handle_ = DEFAULT_DRIVER_HANDLE;
  return true;
}

std::string Driver::get_fan_curve(int channel) {
  if (channel >= Constants::NUM_FANS) {
    return {};
  }
  CurveCommandParameters curve_command = {.channel =
                                              static_cast<uint8_t>(channel),
                                          .curve_type = CurveType::FAN_CURVE};

  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  trx_buf_[0] = (uint8_t)Command::GET_CURVE;
  memcpy(trx_buf_.data() + 1, &curve_command, sizeof(CurveCommandParameters));
  rawhid_send(driver_handle_, trx_buf_.data(), trx_buf_.size(), HID_TIMEOUT);

  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  auto bytes_received = rawhid_recv(driver_handle_, trx_buf_.data(),
                                    trx_buf_.size(), HID_TIMEOUT);
  print_bytes_received(bytes_received, trx_buf_);
  if (bytes_received <= 0) {
    return {};
  }

  uint8_t command;
  memcpy(&command, trx_buf_.data(), sizeof(uint8_t));
  if (command != Command::GET_CURVE) {
    return {};
  }

  memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
  bytes_received = rawhid_recv(driver_handle_, trx_buf_.data(), trx_buf_.size(),
                               HID_TIMEOUT);
  print_bytes_received(bytes_received, trx_buf_);
  if (bytes_received <= 0) {
    return {};
  }

  memcpy(&curve_command, trx_buf_.data(), sizeof(CurveCommandParameters));
  printf("channel %u, curve length: %u\n", curve_command.channel,
         curve_command.curve_length);

  uint16_t packet_offset = sizeof(CurveCommandParameters);
  std::vector<std::tuple<float, float>> points;
  points.reserve(curve_command.curve_length);
  for (uint i = 0; i < curve_command.curve_length; i++) {
    float x, y;
    memcpy(&x, trx_buf_.data() + packet_offset, sizeof(float));
    memcpy(&y, trx_buf_.data() + packet_offset + sizeof(float),
           sizeof(float));
    points.push_back(std::make_tuple(x, y));

    packet_offset += sizeof(float) * 2;
    if (packet_offset + sizeof(float) * 2 >= HID_BUF_SIZE) {
      memset(trx_buf_.data(), 0, sizeof(uint8_t) * trx_buf_.size());
      bytes_received = rawhid_recv(driver_handle_, trx_buf_.data(), trx_buf_.size(),
                                  HID_TIMEOUT);
      print_bytes_received(bytes_received, trx_buf_);
      if (bytes_received <= 0) {
        return {};
      }
      packet_offset = 0;
    }
  }

  nlohmann::json j;
  j["points"] = points;
  return j.dump();
}
} // namespace LB

// int main() {
//   int i, r, num;
//   char c, buf[64];

//   // C-based example is 16C0:0480:FFAB:0200
//   r = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
//   if (r <= 0) {
//     // Arduino-based example is 16C0:0486:FFAB:0200
//     r = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
//     if (r <= 0) {
//       printf("no rawhid device found\n");
//       return -1;
//     }
//   }
//   printf("found rawhid device\n");

//   while (1) {
//     // check if any Raw HID packet has arrived
//     num = rawhid_recv(0, buf, 64, 220);
//     if (num < 0) {
//       printf("\nerror reading, device went offline\n");
//       rawhid_close(0);
//       return 0;
//     }
//     if (num > 0) {
//       printf("\nrecv %d bytes:\n", num);
//       for (i = 0; i < num; i++) {
//         printf("%02X ", buf[i] & 255);
//         if (i % 16 == 15 && i < num - 1)
//           printf("\n");
//       }
//       printf("\n");

//       struct FanParameters fan_params;
//       memcpy(&fan_params, buf, sizeof(struct FanParameters));
//       printf("channel %u, pwm controlled: %u\n", fan_params.channel,
//              fan_params.pwm_controlled);
//     }
//     // check if any input on stdin
//     while ((c = get_keystroke()) >= 32) {
//       printf("\ngot key '%c', sending...\n", c);
//       // buf[0] = c;
//       // for (i=1; i<64; i++) {
//       // 	buf[i] = 0;
//       // }
//       memset(buf, 0, sizeof(char) * 64);
//       struct FanParameters fan_params = {
//           .channel = 0, .pwm_controlled = true};
//       buf[0] = (uint8_t)SET_FAN_PARAMETERS;
//       memcpy(buf + 1, &fan_params, sizeof(struct FanParameters));

//       rawhid_send(0, buf, 64, 100);
//       // memset(buf, 0, sizeof(char) * 64);
//       // float x, y;
//       // x = 0, y = 0;
//       // memcpy(buf, &x, sizeof(float));
//       // memcpy(buf + sizeof(float), &y, sizeof(float));
//       // x = 50, y = 100;
//       // memcpy(buf + sizeof(float) * 2, &x, sizeof(float));
//       // memcpy(buf + sizeof(float) * 3, &y, sizeof(float));
//       // x = 100, y = 0;
//       // memcpy(buf + sizeof(float) * 4, &x, sizeof(float));
//       // memcpy(buf + sizeof(float) * 5, &y, sizeof(float));
//       // rawhid_send(0, buf, 64, 100);
//     }
//   }
// }

// #if defined(OS_LINUX) || defined(OS_MACOSX)
// // Linux (POSIX) implementation of _kbhit().
// // Morgan McGuire, morgan@cs.brown.edu
// static int _kbhit() {
//   static const int STDIN = 0;
//   static int initialized = 0;
//   int bytesWaiting;

//   if (!initialized) {
//     // Use termios to turn off line buffering
//     struct termios term;
//     tcgetattr(STDIN, &term);
//     term.c_lflag &= ~ICANON;
//     tcsetattr(STDIN, TCSANOW, &term);
//     setbuf(stdin, NULL);
//     initialized = 1;
//   }
//   ioctl(STDIN, FIONREAD, &bytesWaiting);
//   return bytesWaiting;
// }
// static char _getch(void) {
//   char c;
//   if (fread(&c, 1, 1, stdin) < 1)
//     return 0;
//   return c;
// }
// #endif

// static char get_keystroke(void) {
//   if (_kbhit()) {
//     char c = _getch();
//     if (c >= 32)
//       return c;
//   }
//   return 0;
// }
