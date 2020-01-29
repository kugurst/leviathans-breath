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

void loop() {
  LB::Config::loop_all();
  LB::Protocol::loop();

  if (print_delay.justFinished()) {
    print_delay.repeat();

#ifndef DISABLE_SERIAL
    // Serial.println(LB::Config::get_fan(0).read_rpm());
    // Serial.println(LB::Config::get_fan(0).is_pwm_controlled());
    // Serial.println(LB::Config::get_temperature_sensor(0).read_temperature());
    // Serial.println(sizeof(LB::LEDControl) * LB::Constants::NUM_LEDS +
    // sizeof(LB::FanControl) * LB::Constants::NUM_FANS);
    // Serial.println(LB::Constants::POINTS_PER_CURVE);
#endif
  }
}

//     /*
//  Example HID to Webcontrol - Basic transmit over HID
//  By: Nathan Seidle (SparkFun Electronics)
//  Date: January 6th, 2014
//  This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

//  This example shows how to transmit a counter over Raw HID.
// */

// #include "usb_rawhid.h"

// //Declare hardware pins
// byte statLED = 13; //Teeny 2.0++ has status LED on pin 6

// long readingTime; //Controls how often we send our trimpot value to the computer
// unsigned int counter = 0; //Used to show packet count sent to computer

// byte outgoingBuffer[16];

// void setup()
// {
//   //Blink to show we're alive
//   pinMode(statLED, OUTPUT);
//   for(int i = 0 ; i < 5 ; i++)
//   {
//     digitalWrite(statLED, HIGH);
//     delay(25);
//     digitalWrite(statLED, HIGH);
//     delay(25);
//   }
//   digitalWrite(statLED, HIGH); //Turn off LED

//   //Fill the buffer with static numbers
//   for(int x = 0 ; x < 16 ; x++)
//     outgoingBuffer[x] = x;

//   readingTime = millis();
// }

// void loop()
// {
//   //Send sensor readings to computer every 30ms, about 33Hz
//   if (millis() - readingTime > 1000) {
//     readingTime += 1000;

//     //Fill the tail end with a counter
//     outgoingBuffer[14] = counter >> 8; //MSB
//     outgoingBuffer[15] = counter & 0xFF; //LSB
//     counter++;

//     //Send the read frame to the computer
//     int response = RawHID.send(outgoingBuffer, 100);

//   }
// }
