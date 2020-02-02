#include <iostream>

#include "driver.h"

int main() {
    LB::Driver driver;
    if (!driver.connect()) {
        return 1;
    }
    std::cout << driver.get_fan_curve(0) << std::endl;
    return 0;
}
