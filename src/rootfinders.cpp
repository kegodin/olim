#include "rootfinders.hpp"

#include <cassert>

#include <gsl/gsl_poly.h>

#include "qroots.hpp"
#include "rpoly.hpp"

void bsearch_rootfinder::find_roots(double const * a, double * roots) const {
  qroots(a, roots);
}

void gsl_rootfinder::find_roots(double const * a, double * roots) const {
  double z[8];
  
  gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc(5);
  gsl_poly_complex_solve(a, 5, w, z);
  gsl_poly_complex_workspace_free(w);

  double root;
  int k = 0;
  for (int i = 0, j = 0; i < 4; ++i, j = 2*i) {
    if (z[j + 1] != 0) {
      continue;
    }
    root = z[j];
    if (root < 0 || root > 1) {
      continue;
    }
    roots[k++] = root;
  }
}

void rpoly_rootfinder::find_roots(double const * a, double * roots) const {
  std::complex<double> z[4];
  bool fail = rpoly(a, 4, z);
  (void) fail;
  assert(!fail);
  for (int i = 0, j = 0; i < 4; ++i) {
    if (z[i].imag() != 0) continue;
    double root = z[i].real();
    if (root < 0 || root > 1) continue;
    roots[j++] = root;
  }
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: