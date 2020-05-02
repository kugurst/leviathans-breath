#include <millisDelay.h>

#include "config.h"
#include "file_manager.h"
#include "protocol.h"
#include "utils.h"

millisDelay print_delay;

void setup() {
#ifndef DISABLE_SERIAL
  Serial.begin(57600);
  while (!Serial)
    ; // wait for Arduino Serial Monitor
#endif

  LB::Config::init();
  LB::FileManager::init();
  LB::FileManager::load_config();
  LB::Protocol::init();

  print_delay.start(250);
}

int print_count = 3;
void loop() {
  LB::Config::loop_all();
  LB::Protocol::loop();

  if (print_delay.justFinished()) {
    print_delay.repeat();

#ifndef DISABLE_SERIAL
    if (print_count > 0) {
      Serial.println(F("Ready!"));
      print_count--;
    }
    // Serial.print(F("LED[0] time controlled: "));
    // Serial.println(LB::Config::get_led_control(0).is_time_controlled());
    // Serial.print(F("LED[1] time controlled: "));
    // Serial.println(LB::Config::get_led_control(1).is_time_controlled());
    // Serial.println(LB::Config::get_fan(0).read_rpm());
    // Serial.println(LB::Config::get_fan(0).is_pwm_controlled());
    //
    Serial.println(LB::Config::get_temperature_sensor(0).read_temperature());
    // Serial.println(sizeof(LB::LEDControl) * LB::Constants::NUM_LEDS +
    // sizeof(LB::FanControl) * LB::Constants::NUM_FANS);
    // Serial.println(LB::Constants::POINTS_PER_CURVE);
#endif
  }
}

// const int pin = 37;

// void setup() {
//   analogWriteResolution(12);
//   pinMode(pin, OUTPUT);
//   pinMode(15, INPUT);
//   analogWriteFrequency(pin, 35000);
//   analogWrite(pin, 4095);
// }

// void loop() {
//   for (int i = 0; i <= 100; i++) {
//     analogWrite(pin, 4095.0f * ((float)i) / 100.0f);
//     delay(100);
//   }
//   for (int i = 100; i >= 0; i--) {
//     analogWrite(pin, 4095.0f * ((float)i) / 100.0f);
//     delay(100);
//   }
// }
