#include "common.hpp"

#include "eikonal/olim.hpp"

using namespace eikonal;

using olim_t = olim8_mp0<>;

TEST (olim8_mp0, trivial_case_works) {
  ASSERT_TRUE(trivial_case_works<olim_t>());
}

TEST (olim8_mp0, adjacent_update_works) {
  ASSERT_TRUE(adjacent_update_works<olim_t>());
}

TEST (olim8_mp0, quadrants_are_correct) {
  ASSERT_TRUE(quadrants_are_correct<olim_t>(sqrt(2)));
}

TEST (olim8_mp0, solution_is_exact_in_factored_square) {
  ASSERT_TRUE(solution_is_exact_in_factored_square<olim_t>(3));
}
