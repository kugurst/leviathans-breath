#include <boost/python.hpp>

#include "driver.hpp"

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
      .def("set_led_parameters", &LB::Driver::set_led_parameters)
  ;
}
