#ifndef __OFFSETS_HPP__
#define __OFFSETS_HPP__

#ifndef __clang__
#ifndef _MSC_VER
#    pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#endif

namespace {

template <int d> constexpr int di[] = {};
template <int d> constexpr int dj[] = {};
template <int d> constexpr int dk[] = {};

inline int d2l(int ddi, int ddj) {
  static constexpr int _lut[] = {7, 0, 4, 3, -1, 1, 6, 2, 5};
  return _lut[3*(ddi + 1) + ddj + 1];
}

inline int d2l(int ddi, int ddj, int ddk) {
  static constexpr int _lut[] = {
    24, 16, 23, 17, 5, 15, 25, 14, 22, 12, 3, 11, 4, -1, 1,
    13, 0, 10, 20, 8, 19, 9, 2, 7, 21, 6, 18
  };
  return _lut[3*3*(ddk + 1) + 3*(ddi + 1) + ddj + 1];
}

}

#include "offsets.impl.hpp"

#ifndef __clang__
#ifndef _MSC_VER
#    pragma GCC diagnostic pop
#endif
#endif

#endif // __OFFSETS_HPP__
