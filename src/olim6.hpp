#ifndef __OLIM6_HPP_HPP__
#define __OLIM6_HPP_HPP__

#include "conics.hpp"
#include "neumann_marcher_3d.hpp"
#include "node_3d.hpp"
#include "olim_update_rules.hpp"

template <class update_rules>
struct olim6: public neumann_marcher_3d<node_3d>, public update_rules {
  using neumann_marcher_3d::neumann_marcher_3d;
private:
  virtual void update_impl(int i, int j, int k, double & T);
};

#include "olim6.impl.hpp"

#endif // __OLIM6_HPP_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
