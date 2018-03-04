#include "olim.hpp"
#include "olim3d.hpp"
#include "olim.test.common.hpp"

using olim_t = olim8_rhr;
using olim3d_t = olim26_rhr;

TEST (olim26_rhr, quadrants_are_correct) {
  quadrants_are_correct<olim3d_t>(sqrt(2));
}

TEST (olim26_rhr, octants_are_correct) {
  octants_are_correct<olim3d_t>(sqrt(2), sqrt(3));
}

TEST (olim26_rhr, planes_are_correct) {
  planes_are_correct<olim_t, olim3d_t>(speed_funcs[0], speed_funcs_3d[0]);
  planes_are_correct<olim_t, olim3d_t>(speed_funcs[1], speed_funcs_3d[1], 9);
}

TEST (olim26_rhr, result_is_symmetric) {
  result_is_symmetric<olim3d_t>(speed_funcs_3d[0]);
  result_is_symmetric<olim3d_t>(speed_funcs_3d[1]);
}

TEST (olim26_rhr, two_by_two_by_three_cells_are_correct) {
  two_by_two_by_three_cells_are_correct<olim3d_t>();
}

TEST (olim26_rhr, plane_boundaries_are_correct) {
  plane_boundaries_are_correct<olim3d_t>();
}
