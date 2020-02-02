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

static char get_keystroke(void);

enum Command {
  GET_CURVE,
  SET_CURVE,
  GET_ALL_RPM,
  GET_ALL_TEMPERATURE,
  GET_ALL_FAN_PARAMETERS,
  SET_FAN_PARAMETERS,
  GET_ALL_LED_PARAMETERS,
  SET_LED_PARAMETERS,
};

enum CurveType { FAN_CURVE, LED_CURVE };

enum CurveChannel { R_CHAN, G_CHAN, B_CHAN };

struct CurveCommandParameters {
  uint8_t channel;
  uint8_t curve_type;
  uint8_t rgb_channel;
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

int main() {
  int i, r, num;
  char c, buf[64];

  // C-based example is 16C0:0480:FFAB:0200
  r = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
  if (r <= 0) {
    // Arduino-based example is 16C0:0486:FFAB:0200
    r = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
    if (r <= 0) {
      printf("no rawhid device found\n");
      return -1;
    }
  }
  printf("found rawhid device\n");

  while (1) {
    // check if any Raw HID packet has arrived
    num = rawhid_recv(0, buf, 64, 220);
    if (num < 0) {
      printf("\nerror reading, device went offline\n");
      rawhid_close(0);
      return 0;
    }
    if (num > 0) {
      printf("\nrecv %d bytes:\n", num);
      for (i = 0; i < num; i++) {
        printf("%02X ", buf[i] & 255);
        if (i % 16 == 15 && i < num - 1)
          printf("\n");
      }
      printf("\n");

      struct FanParameters fan_params;
      memcpy(&fan_params, buf, sizeof(struct FanParameters));
      printf("channel %u, pwm controlled: %u\n", fan_params.channel,
             fan_params.pwm_controlled);
    }
    // check if any input on stdin
    while ((c = get_keystroke()) >= 32) {
      printf("\ngot key '%c', sending...\n", c);
      // buf[0] = c;
      // for (i=1; i<64; i++) {
      // 	buf[i] = 0;
      // }
      memset(buf, 0, sizeof(char) * 64);
      struct FanParameters fan_params = {
          .channel = 0, .pwm_controlled = true};
      buf[0] = (uint8_t)SET_FAN_PARAMETERS;
      memcpy(buf + 1, &fan_params, sizeof(struct FanParameters));

      rawhid_send(0, buf, 64, 100);
      // memset(buf, 0, sizeof(char) * 64);
      // float x, y;
      // x = 0, y = 0;
      // memcpy(buf, &x, sizeof(float));
      // memcpy(buf + sizeof(float), &y, sizeof(float));
      // x = 50, y = 100;
      // memcpy(buf + sizeof(float) * 2, &x, sizeof(float));
      // memcpy(buf + sizeof(float) * 3, &y, sizeof(float));
      // x = 100, y = 0;
      // memcpy(buf + sizeof(float) * 4, &x, sizeof(float));
      // memcpy(buf + sizeof(float) * 5, &y, sizeof(float));
      // rawhid_send(0, buf, 64, 100);
    }
  }
}

#if defined(OS_LINUX) || defined(OS_MACOSX)
// Linux (POSIX) implementation of _kbhit().
// Morgan McGuire, morgan@cs.brown.edu
static int _kbhit() {
  static const int STDIN = 0;
  static int initialized = 0;
  int bytesWaiting;

  if (!initialized) {
    // Use termios to turn off line buffering
    struct termios term;
    tcgetattr(STDIN, &term);
    term.c_lflag &= ~ICANON;
    tcsetattr(STDIN, TCSANOW, &term);
    setbuf(stdin, NULL);
    initialized = 1;
  }
  ioctl(STDIN, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}
static char _getch(void) {
  char c;
  if (fread(&c, 1, 1, stdin) < 1)
    return 0;
  return c;
}
#endif

static char get_keystroke(void) {
  if (_kbhit()) {
    char c = _getch();
    if (c >= 32)
      return c;
  }
  return 0;
}
