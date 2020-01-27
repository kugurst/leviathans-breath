#ifndef __UTILS_H__
#define __UTILS_H__

#define CUSTOM_TACH
#define FIXED_POINTS_USE_NAMESPACE
#define DISABLE_SERIAL

#include <functional>

// #define FASTRUN __attribute__((section(".fastrun")))

// to use `std::function`, we need this definition
namespace std {
void __throw_bad_function_call() {
  while (1)
    ;
}
void __throw_length_error(char const *) {
  while (1)
    ;
}
} // namespace std

extern "C" {
int _getpid() { return -1; }
int _kill(int pid, int sig) { return -1; }
}

namespace LB {
using SetDacFunc = std::function<void(uint16_t level)>;
}

#endif
