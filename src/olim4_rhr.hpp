#ifndef __OLIM4_RHR_HPP_HPP__
#define __OLIM4_RHR_HPP_HPP__

#include "neumann_marcher.hpp"
#include "node.hpp"

struct olim4_rhr: public neumann_marcher<node>
{
  using neumann_marcher::neumann_marcher;
private:
  virtual void update_impl(int i, int j, double & T);
};

#endif // __OLIM4_RHR_HPP_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
