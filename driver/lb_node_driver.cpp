#include "rawhid/driver.hpp"

// #include "rawhid/hid_LINUX.c"

#include <nan.h>

const int maxValue = 10;
int numberOfCalls = 0;

LB::Driver driver;

NAN_METHOD(Connect) { info.GetReturnValue().Set(driver.connect()); }

NAN_METHOD(IsConnected) { info.GetReturnValue().Set(driver.is_connected()); }

NAN_METHOD(Sync) { info.GetReturnValue().Set(driver.sync()); }

NAN_METHOD(GetFanCurve) {
  if (info.Length() < 1) {
    return;
  } else if (!info[0]->IsNumber()) {
    Nan::ThrowError("Argument must be a number");
    return;
  }

  const auto channel = Nan::To<int32_t>(info[0]).FromJust();
  if (channel >= LB::Constants::NUM_FANS) {
    Nan::ThrowError("Selected fan channel is greater than the number of fans");
    return;
  }

  const auto message =
      Nan::New<v8::String>(driver.get_fan_curve(channel)).ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_METHOD(SetFanCurve) {
  if (info.Length() < 2) {
    return;
  } else if (!info[0]->IsNumber()) {
    return;
  } else if (!info[1]->IsString()) {
    return;
  }

  Nan::Utf8String curve(info[1]);
  int len = curve.length();
  if (len <= 0) {
    return Nan::ThrowTypeError("Curve must be a non-empty string");
  }
  const std::string curve_copy(*curve, len);
  const auto channel = Nan::To<int32_t>(info[0]).FromJust();
  if (channel >= LB::Constants::NUM_FANS) {
    Nan::ThrowError("Selected fan channel is greater than the number of fans");
    return;
  }

  info.GetReturnValue().Set(driver.send_fan_curve(channel, curve_copy));
}

NAN_METHOD(GetLedCurve) {
  if (info.Length() < 1) {
    return;
  } else if (!info[0]->IsNumber()) {
    Nan::ThrowError("Argument must be a number");
    return;
  }

  const auto channel = Nan::To<int32_t>(info[0]).FromJust();
  if (channel >= LB::Constants::NUM_LEDS) {
    Nan::ThrowError("Selected LED channel is greater than the number of LEDs");
    return;
  }

  const auto message =
      Nan::New<v8::String>(driver.get_led_curve(channel)).ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_METHOD(SetLedCurve) {
  if (info.Length() < 2) {
    return;
  } else if (!info[0]->IsNumber()) {
    return;
  } else if (!info[1]->IsString()) {
    return;
  }

  Nan::Utf8String curve(info[1]);
  int len = curve.length();
  if (len <= 0) {
    return Nan::ThrowTypeError("Curve must be a non-empty string");
  }
  const std::string curve_copy(*curve, len);
  const auto channel = Nan::To<int32_t>(info[0]).FromJust();
  if (channel >= LB::Constants::NUM_LEDS) {
    Nan::ThrowError("Selected LED channel is greater than the number of LEDs");
    return;
  }

  info.GetReturnValue().Set(driver.send_led_curve(channel, curve_copy));
}

NAN_METHOD(GetAllFanRpms) {
  const auto message =
      Nan::New<v8::String>(driver.get_all_fan_rpms()).ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_METHOD(GetAllTemperatures) {
  const auto message =
      Nan::New<v8::String>(driver.get_all_temperatures()).ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_METHOD(GetAllFanParameters) {
  const auto message =
      Nan::New<v8::String>(driver.get_all_fan_parameters()).ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_METHOD(SetFanParameters) {
  if (info.Length() < 3) {
    return;
  } else if (!info[0]->IsNumber()) {
    return;
  } else if (!info[1]->IsBoolean()) {
    return;
  } else if (!info[2]->IsNumber()) {
    return;
  }

  const auto channel = Nan::To<int32_t>(info[0]).FromJust();
  if (channel >= LB::Constants::NUM_FANS) {
    Nan::ThrowError("Selected fan channel is greater than the number of fans");
    return;
  }
  const auto pwm_controlled = Nan::To<bool>(info[1]).FromJust();
  const auto sensor_idx = Nan::To<int32_t>(info[2]).FromJust();
  if (channel >= LB::Constants::NUM_TEMPERATURE_SENSORS) {
    Nan::ThrowError("Selected temperature sensor is out of range");
    return;
  }

  info.GetReturnValue().Set(
      driver.set_fan_parameters(channel, pwm_controlled, sensor_idx));
}

NAN_METHOD(GetAllLedParameters) {
  const auto message =
      Nan::New<v8::String>(driver.get_all_led_parameters()).ToLocalChecked();
  info.GetReturnValue().Set(message);
}

NAN_METHOD(SetLedParameters) {
  if (info.Length() < 4) {
    return;
  } else if (!info[0]->IsNumber()) {
    return;
  } else if (!info[1]->IsBoolean()) {
    return;
  } else if (!info[2]->IsNumber()) {
    return;
  } else if (!info[3]->IsNumber()) {
    return;
  }

  const auto channel = Nan::To<int32_t>(info[0]).FromJust();
  if (channel >= LB::Constants::NUM_FANS) {
    Nan::ThrowError("Selected fan channel is greater than the number of fans");
    return;
  }
  const auto time_controlled = Nan::To<bool>(info[1]).FromJust();
  const auto speed_multiplier = Nan::To<double>(info[2]).FromJust();
  const auto sensor_idx = Nan::To<int32_t>(info[3]).FromJust();
  if (channel >= LB::Constants::NUM_TEMPERATURE_SENSORS) {
    Nan::ThrowError("Selected temperature sensor is out of range");
    return;
  }

  info.GetReturnValue().Set(driver.set_led_parameters(
      channel, time_controlled, static_cast<float>(speed_multiplier),
      sensor_idx));
}

NAN_MODULE_INIT(Initialize) {
  NAN_EXPORT(target, Connect);
  NAN_EXPORT(target, IsConnected);
  NAN_EXPORT(target, Sync);
  NAN_EXPORT(target, GetFanCurve);
  NAN_EXPORT(target, SetFanCurve);
  NAN_EXPORT(target, GetLedCurve);
  NAN_EXPORT(target, SetLedCurve);
  NAN_EXPORT(target, GetAllFanRpms);
  NAN_EXPORT(target, GetAllTemperatures);
  NAN_EXPORT(target, GetAllFanParameters);
  NAN_EXPORT(target, SetFanParameters);
  NAN_EXPORT(target, GetAllLedParameters);
  NAN_EXPORT(target, SetLedParameters);
}

NODE_MODULE(leviathans_breath, Initialize)
