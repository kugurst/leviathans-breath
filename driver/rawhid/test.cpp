#include <iostream>
#include <limits>

#include <gtest/gtest.h>

#include "nlohmann/json.hpp"

#include "driver.h"

namespace LB {
static Driver driver;
class SetupEnvironment : public ::testing::Environment {
public:
  ~SetupEnvironment() override{};
  void SetUp() override { ASSERT_TRUE(driver.connect()); }
  void TearDown() override { ASSERT_TRUE(driver.disconnect()); }
};

class DriverTest : public ::testing::Test {
protected:
  void SetUp() override { ASSERT_TRUE(driver.sync()); }
};

TEST_F(DriverTest, Sync) { ASSERT_TRUE(driver.sync()); }

TEST_F(DriverTest, GetFanCurve) {
  for (int i = 0; i < Constants::NUM_FANS; i++) {
    auto points = driver.get_fan_curve(i);
    ASSERT_TRUE(points.length() > 0);
    nlohmann::json j = nlohmann::json::parse(points);
    auto parsed_points =
        j["points"].get<std::vector<std::tuple<float, float>>>();
    ASSERT_TRUE(parsed_points.size() > 0);
  }
}

TEST_F(DriverTest, SendFanCurve) {
  std::vector<std::tuple<float, float>> points;
  points.reserve(Constants::POINTS_PER_CURVE);
  for (auto i = 0; i < points.capacity(); i++) {
    points.push_back({i % std::numeric_limits<uint8_t>::max(),
                      i % std::numeric_limits<uint8_t>::max()});
  }
  nlohmann::json json_points;
  json_points["points"] = points;
  auto points_str = json_points.dump();

  for (int i = 0; i < Constants::NUM_FANS; i++) {
    ASSERT_TRUE(driver.send_fan_curve(i, points_str));
    auto test_points = driver.get_fan_curve(i);
    ASSERT_TRUE(test_points.length() > 0);
    nlohmann::json j = nlohmann::json::parse(test_points);
    auto parsed_points =
        j["points"].get<std::vector<std::tuple<float, float>>>();
    ASSERT_TRUE(parsed_points.size() > 0);
    ASSERT_EQ(parsed_points.size(), points.size());
    for (auto i = 0; i < points.size(); i++) {
#ifdef DEBUG
      std::cout << "[" << std::get<0>(parsed_points[i]) << ", "
                << std::get<1>(parsed_points[i]) << "], ["
                << std::get<0>(points[i]) << ", " << std::get<1>(points[i])
                << "]" << std::endl;
#endif
      ASSERT_EQ(parsed_points[i], points[i]);
    }
  }
}

TEST_F(DriverTest, GetLedCurve) {
  for (int i = 0; i < Constants::NUM_LEDS; i++) {
    auto points = driver.get_led_curve(i);
    ASSERT_TRUE(points.length() > 0);
    nlohmann::json j = nlohmann::json::parse(points);
    auto parsed_points =
        j["points"]["r"].get<std::vector<std::tuple<float, float>>>();
    ASSERT_TRUE(parsed_points.size() > 0);
    parsed_points =
        j["points"]["g"].get<std::vector<std::tuple<float, float>>>();
    ASSERT_TRUE(parsed_points.size() > 0);
    parsed_points =
        j["points"]["b"].get<std::vector<std::tuple<float, float>>>();
    ASSERT_TRUE(parsed_points.size() > 0);
  }
}

TEST_F(DriverTest, SendLedCurve) {
  std::vector<std::tuple<float, float>> points;
  points.reserve(Constants::POINTS_PER_CURVE);
  for (auto i = 0; i < points.capacity(); i++) {
    points.push_back({i % std::numeric_limits<uint8_t>::max(),
                      i % std::numeric_limits<uint8_t>::max()});
  }
  nlohmann::json json_points;
  json_points["points"]["r"] = points;
  json_points["points"]["g"] = points;
  json_points["points"]["b"] = points;
  auto points_str = json_points.dump();

  for (int i = 0; i < Constants::NUM_LEDS; i++) {
    ASSERT_TRUE(driver.send_led_curve(i, points_str));
    auto test_points = driver.get_led_curve(i);
    ASSERT_TRUE(test_points.length() > 0);
    nlohmann::json j = nlohmann::json::parse(test_points);
    std::array<std::vector<std::tuple<float, float>>,
               Constants::NUM_LED_CHANNELS>
        parsed_points_set;
    parsed_points_set[0] =
        j["points"]["r"].get<std::vector<std::tuple<float, float>>>();
    parsed_points_set[1] =
        j["points"]["g"].get<std::vector<std::tuple<float, float>>>();
    parsed_points_set[2] =
        j["points"]["b"].get<std::vector<std::tuple<float, float>>>();
    for (auto &parsed_points : parsed_points_set) {
      ASSERT_TRUE(parsed_points.size() > 0);
      ASSERT_EQ(parsed_points.size(), points.size());
      for (auto i = 0; i < points.size(); i++) {
#ifdef DEBUG
        std::cout << "[" << std::get<0>(parsed_points[i]) << ", "
                  << std::get<1>(parsed_points[i]) << "], ["
                  << std::get<0>(points[i]) << ", " << std::get<1>(points[i])
                  << "]" << std::endl;
#endif
        ASSERT_EQ(parsed_points[i], points[i]);
      }
    }
  }
}

TEST_F(DriverTest, GetFanRpms) {
  auto fan_rpms = driver.get_all_fan_rpms();
  ASSERT_TRUE(fan_rpms.length() > 0);
  nlohmann::json j = nlohmann::json::parse(fan_rpms);
  ASSERT_TRUE(j["rpms"].size() > 0);

  auto rpms = j["rpms"].get<std::vector<float>>();
  ASSERT_TRUE(rpms.size() > 0);
}

TEST_F(DriverTest, GetTemperatures) {
  auto temperatures = driver.get_all_temperatures();
  ASSERT_TRUE(temperatures.length() > 0);
  nlohmann::json j = nlohmann::json::parse(temperatures);
  ASSERT_TRUE(j["temperatures"].size() > 0);

  auto temperature_vec = j["temperatures"].get<std::vector<float>>();
  ASSERT_TRUE(temperature_vec.size() > 0);

#ifdef DEBUG
  std::cout << temperatures << std::endl;
#endif
}

TEST_F(DriverTest, GetFanParameters) {
  auto all_fans = driver.get_all_fan_parameters();
  ASSERT_TRUE(all_fans.length() > 0);
  nlohmann::json j = nlohmann::json::parse(all_fans);
  ASSERT_TRUE(j["fans"].size() > 0);

  for (auto &fan_params : j["fans"]) {
    ASSERT_NO_THROW(fan_params["channel"].get<int>());
    ASSERT_NO_THROW(fan_params["pwm"].get<float>());
    ASSERT_NO_THROW(fan_params["pwm_controlled"].get<bool>());
    ASSERT_NO_THROW(fan_params["voltage"].get<float>());
  }
}

TEST_F(DriverTest, GetLedParameters) {
  auto all_leds = driver.get_all_led_parameters();
  ASSERT_TRUE(all_leds.length() > 0);
  nlohmann::json j = nlohmann::json::parse(all_leds);
  ASSERT_TRUE(j["leds"].size() > 0);

  for (auto &led_params : j["leds"]) {
    ASSERT_NO_THROW(led_params["channel"].get<int>());
    ASSERT_NO_THROW(led_params["time_controlled"].get<bool>());
    ASSERT_NO_THROW(led_params["speed_multiplier"].get<float>());
    ASSERT_NO_THROW(led_params["r_brightness"].get<float>());
    ASSERT_NO_THROW(led_params["g_brightness"].get<float>());
    ASSERT_NO_THROW(led_params["b_brightness"].get<float>());
  }
}
} // namespace LB

int main(int argc, char **argv) {
  ::testing::AddGlobalTestEnvironment(new LB::SetupEnvironment());
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
