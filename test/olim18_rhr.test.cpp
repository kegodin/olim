#include "common.hpp"
#include "olim.hpp"
#include "olim3d.hpp"
#include "olim.test.common.hpp"

using olim_t = olim8_rhr<>;
using olim3d_t = olim18_rhr<>;

TEST (olim18_rhr, quadrants_are_correct) {
  ASSERT_TRUE(quadrants_are_correct<olim3d_t>(sqrt2));
}

TEST (olim18_rhr, octants_are_correct) {
  ASSERT_TRUE(octants_are_correct<olim3d_t>(sqrt2, sqrt2 + 1.0/sqrt3));
}

TEST (olim18_rhr, two_by_two_by_three_cells_are_correct) {
  ASSERT_TRUE(two_by_two_by_three_cells_are_correct<olim3d_t>());
}

TEST (olim18_rhr, plane_boundaries_are_correct) {
  ASSERT_TRUE(plane_boundaries_are_correct<olim3d_t>());
}

TEST (olim18_rhr, solution_is_exact_in_factored_region) {
  ASSERT_TRUE(solution_is_exact_in_factored_square<olim3d_t>(5));
}
