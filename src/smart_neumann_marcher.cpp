#include "smart_neumann_marcher.hpp"

// neighbor order: N, E, S, W (clockwise from north)

int smart_neumann_marcher::di[] = {-1, 0, 1, 0};
int smart_neumann_marcher::dj[] = {0, 1, 0, -1};

void smart_neumann_marcher::stage_neighbors_impl(abstract_node * n) {
  int i = static_cast<smart_node *>(n)->get_i();
  int j = static_cast<smart_node *>(n)->get_j();

  for (int k = 0; k < 4; ++k) {
    stage_neighbor(i + di[k], j + dj[k]);
  }

  int a, b;
  for (int k = 0; k < 4; ++k) {
    a = i + di[k], b = j + dj[k];
    if (in_bounds(a, b) && !operator()(a, b).is_valid()) {
      update_node_value(a, b);
    }
  }
}

void smart_neumann_marcher::get_valid_neighbors(int i, int j, abstract_node ** nb) {
  int a, b;
  for (int k = 0; k < 4; ++k) {
    a = i + di[k], b = j + dj[k];
    if (is_valid(a, b)) {
      nb[k] = &operator()(a, b);
    }
  }
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: