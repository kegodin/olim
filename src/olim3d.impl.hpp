#ifndef __OLIM3D_IMPL_HPP__
#define __OLIM3D_IMPL_HPP__

#if COLLECT_STATS
#    include <cstdio>
#endif

#include <src/config.hpp>
#include "offsets.hpp"
#if COLLECT_STATS
#  include "update_rules.utils.hpp"
#endif

#define __di(l) di<3>[l]
#define __dj(l) dj<3>[l]
#define __dk(l) dk<3>[l]

/**
 * We only need to do triangle or line updates that lie in the xy-,
 * xz-, or yz-planes for every other octant using the preceding
 * enumeration of the octants (otherwise we double count these
 * updates). This function returns true if the passed octant requires
 * these updates.
 */
inline bool should_do_xyz_planar_updates(int octant) {
  static constexpr char parity[8] = {0, 1, 1, 0, 1, 0, 0, 1};
  return static_cast<bool>(parity[octant]);
}

namespace ind {
  // degree 1
  constexpr int N = 0;
  constexpr int E = 1;
  constexpr int U = 2;
  constexpr int S = 3;
  constexpr int W = 4;
  constexpr int D = 5;
  
  // degree 2
  constexpr int UN = 6;
  constexpr int UE = 7;
  constexpr int US = 8;
  constexpr int UW = 9;
  constexpr int NE = 10;
  constexpr int SE = 11;
  constexpr int SW = 12;
  constexpr int NW = 13;
  constexpr int DN = 14;
  constexpr int DE = 15;
  constexpr int DS = 16;
  constexpr int DW = 17;
  
  // degree 3
  constexpr int UNE = 18;
  constexpr int USE = 19;
  constexpr int USW = 20;
  constexpr int UNW = 21;
  constexpr int DNE = 22;
  constexpr int DSE = 23;
  constexpr int DSW = 24;
  constexpr int DNW = 25;
}

constexpr int oct2inds[8][7] = {
  {ind::D, ind::DS, ind::S, ind::SE, ind::E, ind::DE, ind::DSE},
  {ind::D, ind::DS, ind::S, ind::SW, ind::W, ind::DW, ind::DSW},
  {ind::D, ind::DN, ind::N, ind::NE, ind::E, ind::DE, ind::DNE},
  {ind::D, ind::DN, ind::N, ind::NW, ind::W, ind::DW, ind::DNW},
  {ind::U, ind::US, ind::S, ind::SE, ind::E, ind::UE, ind::USE},
  {ind::U, ind::US, ind::S, ind::SW, ind::W, ind::UW, ind::USW},
  {ind::U, ind::UN, ind::N, ind::NE, ind::E, ind::UE, ind::UNE},
  {ind::U, ind::UN, ind::N, ind::NW, ind::W, ind::UW, ind::UNW}
};

#define P001 1
#define P010 2
#define P011 3
#define P100 4
#define P101 5
#define P110 6
#define P111 7

#if COLLECT_STATS
#  define UPDATE_LINE_STATS(d) do {    \
    node_stats.inc_line_updates(d);   \
  } while (0)
#else
#  define UPDATE_LINE_STATS(d) do {} while (0)
#endif

#define LINE(i, d) do {                                             \
    if (nb[i]) {                                                    \
      T = min(T, this->template line<d>(VAL(i), SPEED_ARGS(i), h)); \
      UPDATE_LINE_STATS(d);                                         \
    }                                                               \
  } while (0)

#if COLLECT_STATS
#  define UPDATE_TRI_STATS(p0, p1) do {                 \
    node_stats.inc_tri_updates(weight(p0), weight(p1)); \
  } while (0)
#else
#  define UPDATE_TRI_STATS(p0, p1) do {} while (0)
#endif

#define TRI(i, j, p0, p1) do {                  \
    int l0 = inds[i], l1 = inds[j];             \
    if (nb[l0] && nb[l1]) {                     \
      T = min(                                  \
        T,                                      \
        this->tri(                              \
          VAL(l0),                              \
          VAL(l1),                              \
          SPEED_ARGS(l0, l1),                   \
          h,                                    \
          ffvec<P ## p0> {},                    \
          ffvec<P ## p1> {}));                  \
      UPDATE_TRI_STATS(P ## p0, P ## p1);       \
    }                                           \
  } while (0)

#if COLLECT_STATS
#  define UPDATE_TETRA_STATS(p0, p1, p2) do {                           \
    node_stats.inc_tetra_updates(weight(p0), weight(p1), weight(p2));   \
  } while (0)
#else
#  define UPDATE_TETRA_STATS(p0, p1, p2) do {} while (0)
#endif

#define TETRA(i, j, k, p0, p1, p2) do {                 \
    int l0 = inds[i], l1 = inds[j], l2 = inds[k];       \
    if (nb[l0] && nb[l1] && nb[l2]) {                   \
      T = min(                                          \
        T,                                              \
        this->tetra(                                    \
          VAL(l0),                                      \
          VAL(l1),                                      \
          VAL(l2),                                      \
          SPEED_ARGS(l0, l1, l2),                       \
          h,                                            \
          ffvec<P ## p0> {},                            \
          ffvec<P ## p1> {},                            \
          ffvec<P ## p2> {}));                          \
      UPDATE_TETRA_STATS(P ## p0, P ## p1, P ## p2);    \
    }                                                   \
  } while (0)

#if COLLECT_STATS

template <class node, class line_updates, class tri_updates,
          class tetra_updates, class groups>
void olim3d<
  node, line_updates, tri_updates, tetra_updates,
  groups>::dump_stats() const
{
  for (int k = 0; k < this->get_depth(); ++k) {
    for (int j = 0; j < this->get_width(); ++j) {
      for (int i = 0; i < this->get_height(); ++i) {
        auto const & stats = this->get_node_stats(i, j, k);
        printf("%d, %d, %d: line = %d, tri = %d, tetra = %d\n",
               i, j, k,
               stats.num_line_updates(),
               stats.num_tri_updates(),
               stats.num_tetra_updates());
      }
    }
  }
}

#endif // COLLECT_STATS

template <class node, class line_updates, class tri_updates,
          class tetra_updates, class groups>
void olim3d<
  node, line_updates, tri_updates, tetra_updates,
  groups>::get_valid_neighbors(int i, int j, int k, abstract_node ** nb)
{
  /**
   * TODO: conditionally only stage the neighbors that are necessary
   * based on which groups are being used
   */
  int a, b, c;
  for (int l = 0; l < groups::nneib; ++l) {
    a = i + __di(l), b = j + __dj(l), c = k + __dk(l);
    if (this->in_bounds(a, b, c) && this->is_valid(a, b, c)) {
      nb[l] = &this->operator()(a, b, c);
    }
  }
}

template <class node, class line_updates, class tri_updates,
          class tetra_updates, class groups>
void olim3d<
  node, line_updates, tri_updates,
  tetra_updates, groups>::stage_neighbors_impl(abstract_node * n)
{
  /**
   * TODO: conditionally only stage the neighbors that are necessary
   * based on which groups are being used
   */

  int i = static_cast<node *>(n)->get_i();
  int j = static_cast<node *>(n)->get_j();
  int k = static_cast<node *>(n)->get_k();
#if PRINT_UPDATES
  printf("olim3d::stage_neighbors_impl(i = %d, j = %d, k = %d)\n", i, j, k);
#endif

  for (int l = 0; l < groups::nneib; ++l) {
    this->stage(i + __di(l), j + __dj(l), k + __dk(l));
  }

  int a, b, c;
  for (int l = 0; l < groups::nneib; ++l) {
    a = i + __di(l), b = j + __dj(l), c = k + __dk(l);
    if (this->in_bounds(a, b, c) && !this->operator()(a, b, c).is_valid()) {
      this->update(a, b, c);
    }
  }
}

template <class node, class line_updates, class tri_updates,
          class tetra_updates, class groups>
void olim3d<
  node, line_updates, tri_updates,
  tetra_updates, groups>::update_impl(int i, int j, int k, double & T)
{
  using std::min;
#if PRINT_UPDATES
  printf("olim3d::update_impl(i = %d, j = %d, k = %d)\n", i, j, k);
#endif

  abstract_node * nb[groups::nneib];
  memset(nb, 0x0, groups::nneib*sizeof(abstract_node *));
  get_valid_neighbors(i, j, k, nb);

  double h = this->get_h(), s = this->get_speed(i, j, k), s_[groups::nneib];
  for (int l = 0; l < groups::nneib; ++l) {
    if (nb[l]) {
      s_[l] = this->get_speed(i + __di(l), j + __dj(l), k + __dk(l));
    }
  }

#if COLLECT_STATS
  auto & node_stats = get_node_stats(i, j, k);
#endif

  /**
   * Line updates:
   */
  for (int l = 0; l < 6; ++l) LINE(l, 1);
  if (do_line2_updates) for (int l = 6; l < 18; ++l) LINE(l, 2);
  if (do_line3_updates) for (int l = 18; l < 26; ++l) LINE(l, 3);

  int const * inds;
  for (int octant = 0; octant < 8; ++octant) {
    inds = oct2inds[octant];

    /**
     * Triangle updates:
     */
    if (should_do_xyz_planar_updates(octant)) {
      if (do_tri11_updates) {
        TRI(0, 2, 001, 010);
        TRI(2, 4, 010, 100);
        TRI(4, 0, 100, 001);
      }
      if (do_tri12_updates) {
        TRI(0, 1, 001, 011);
        TRI(2, 1, 010, 011);
        TRI(2, 3, 010, 110);
        TRI(4, 3, 100, 110);
        TRI(4, 5, 100, 101);
        TRI(0, 5, 001, 101);
      }
    }
    if (do_tri13_updates) {
      TRI(0, 6, 001, 111);
      TRI(2, 6, 010, 111);
      TRI(4, 6, 100, 111);
    }
    if (do_tri22_updates) {
      TRI(1, 3, 011, 110);
      TRI(3, 5, 110, 101);
      TRI(5, 1, 101, 011);
    }
    if (do_tri23_updates) {
      TRI(1, 6, 011, 111);
      TRI(3, 6, 110, 111);
      TRI(5, 6, 101, 111);
    }

    /**
     * Tetrahedron updates:
     */
    if (groups::group_I) {
      TETRA(1, 2, 3, 011, 010, 110);
      TETRA(3, 4, 5, 110, 100, 101);
      TETRA(5, 0, 1, 101, 001, 011);
    }
    if (groups::group_II) {
      TETRA(0, 1, 3, 001, 011, 110);
      TETRA(1, 2, 4, 011, 010, 100);
      TETRA(2, 3, 5, 010, 110, 101);
      TETRA(3, 4, 0, 110, 100, 001);
      TETRA(4, 5, 1, 100, 101, 011);
      TETRA(5, 0, 2, 101, 001, 010);
    }
    if (groups::group_III) {
      TETRA(0, 1, 4, 001, 011, 100);
      TETRA(1, 2, 5, 011, 010, 101);
      TETRA(2, 3, 0, 010, 110, 001);
      TETRA(3, 4, 1, 110, 100, 011);
      TETRA(4, 5, 2, 100, 101, 010);
      TETRA(5, 0, 3, 101, 001, 110);
    }
    if (groups::group_IV_a) {
      TETRA(0, 2, 4, 001, 010, 100);
    }
    if (groups::group_IV_b) {
      TETRA(1, 3, 5, 011, 110, 101);
    }
    if (groups::group_V) {
      TETRA(0, 1, 6, 001, 011, 111);
      TETRA(1, 2, 6, 011, 010, 111);
      TETRA(2, 3, 6, 010, 110, 111);
      TETRA(3, 4, 6, 110, 100, 111);
      TETRA(4, 5, 6, 100, 101, 111);
      TETRA(5, 0, 6, 101, 001, 111);
    }
    if (groups::group_VI_a) {
      TETRA(0, 2, 6, 001, 010, 111);
      TETRA(2, 4, 6, 010, 100, 111);
      TETRA(4, 0, 6, 100, 001, 111);
    }
    if (groups::group_VI_b) {
      TETRA(1, 3, 6, 011, 110, 111);
      TETRA(3, 5, 6, 110, 101, 111);
      TETRA(5, 1, 6, 101, 011, 111);
    }
  }

#if PRINT_UPDATES
  printf("olim3d::update_impl: T <- %g\n", T);
#endif
}

#undef LINE
#undef TRI
#undef TETRA
#undef P001
#undef P010
#undef P011
#undef P100
#undef P101
#undef P110
#undef P111

#undef __di
#undef __dj
#undef __dk

#if COLLECT_STATS

template <class node, class line_updates, class tri_updates,
          class tetra_updates, class groups>
olim3d_node_stats &
olim3d<node, line_updates, tri_updates, tetra_updates,
       groups>::get_node_stats(int i, int j, int k)
{
  return _node_stats[this->get_height()*(this->get_width()*k + j) + i];
}

template <class node, class line_updates, class tri_updates,
          class tetra_updates, class groups>
olim3d_node_stats const &
olim3d<node, line_updates, tri_updates, tetra_updates,
       groups>::get_node_stats(int i, int j, int k) const
{
  return _node_stats[this->get_height()*(this->get_width()*k + j) + i];
}

#endif // COLLECT_STATS

#endif // __OLIM3D_IMPL_HPP__
