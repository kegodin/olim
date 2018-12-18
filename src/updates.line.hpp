#ifndef __UPDATE_RULES_LINE_UPDATES_HPP__
#define __UPDATE_RULES_LINE_UPDATES_HPP__

#include "cost_funcs.hpp"

namespace updates {

template <cost_func F, int d>
struct line_bv {
  inline double operator()(double u0, double s, double s0, double h) const {
    return u0 + (F == RHR ? s : (s + s0)/2)*h*sqrt(d);
  }
};

template <cost_func F, int n>
struct line {
  double operator()(double const * p0, double u0, double s, double s0,
                    double h) const {
    return u0 + (F == RHR ? s : (s + s0)/2)*h*norm2<n>(p0);
  }
};

}

#endif // __UPDATE_RULES_LINE_UPDATES_HPP__
