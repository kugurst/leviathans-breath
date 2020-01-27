#ifndef __CIRCULAR_ARRAY_H__
#define __CIRCULAR_ARRAY_H__

#include <algorithm>
#include <array>
#include <iterator>

template <typename T, size_t NUM_ELEMS> class CircularArray {
public:
  const std::array<T, NUM_ELEMS> &get_buffer() { return backing_store_; };

  void push_back(T elem) {
    backing_store_[head_] = elem;
    head_ = (head_ + 1) % NUM_ELEMS;
  };

private:
  std::array<T, NUM_ELEMS> backing_store_;
  uint32_t head_ = 0;
};

#endif
