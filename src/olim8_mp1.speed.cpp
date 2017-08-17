#include "olim8_mp1.hpp"
#include "speed_funcs.hpp"

int main() {
  olim8_mp1_bsearch m(201, 201, 0.01, s1, 1.0, 1.0);
  m.add_boundary_node(100, 100);
  m.run();
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: