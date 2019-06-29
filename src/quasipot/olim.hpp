#pragma once

namespace quasipot {

template <ordering ord>
struct olim: public marcher<olim<ord>, 2, ord>
{
  using marcher<olim<ord>, 2, ord>::marcher;

OLIM_PRIVATE:

  void update_impl(int lin, state s, double & U);
};

}
