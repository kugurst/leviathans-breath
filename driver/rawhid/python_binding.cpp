#include <boost/python.hpp>

#include "constants.hpp"
#include "driver.hpp"

inline int NUM_FANS() { return LB::Constants::NUM_FANS; }
inline int NUM_LEDS() { return LB::Constants::NUM_LEDS; }
inline int NUM_LED_CHANNELS() { return LB::Constants::NUM_LED_CHANNELS; }
inline int NUM_TEMPERATURE_SENSORS() { return LB::Constants::NUM_TEMPERATURE_SENSORS; }
inline int POINTS_PER_CURVE() { return LB::Constants::POINTS_PER_CURVE; }

BOOST_PYTHON_MODULE(leviathans_breath) {
  using namespace boost::python;

  class_<LB::Driver>("Driver")
      .def("connect", &LB::Driver::connect)
      .def("is_connected", &LB::Driver::is_connected)
      .def("sync", &LB::Driver::sync)
      .def("get_fan_curve", &LB::Driver::get_fan_curve)
      .def("send_fan_curve", &LB::Driver::send_fan_curve)
      .def("get_led_curve", &LB::Driver::get_led_curve)
      .def("send_led_curve", &LB::Driver::send_led_curve)
      .def("get_all_fan_rpms", &LB::Driver::get_all_fan_rpms)
      .def("get_all_temperatures", &LB::Driver::get_all_temperatures)
      .def("get_all_fan_parameters", &LB::Driver::get_all_fan_parameters)
      .def("set_fan_parameters", &LB::Driver::set_fan_parameters)
      .def("get_all_led_parameters", &LB::Driver::get_all_led_parameters)
      .def("set_led_parameters", &LB::Driver::set_led_parameters);

  def("NUM_FANS", NUM_FANS);
  def("NUM_LEDS", NUM_LEDS);
  def("NUM_LED_CHANNELS", NUM_LED_CHANNELS);
  def("NUM_TEMPERATURE_SENSORS", NUM_TEMPERATURE_SENSORS);
  def("POINTS_PER_CURVE", POINTS_PER_CURVE);
}
