#ifndef __MARCHER_3D_IMPL_HPP__
#define __MARCHER_3D_IMPL_HPP__

#include <src/config.hpp>

#include <assert.h>
#include <math.h>

#include "offsets.hpp"
#include "updates.line.hpp"

#define __di(l) di<3>[l]
#define __dj(l) dj<3>[l]
#define __dk(l) dk<3>[l]

#define __x(l) (h*l - x0)
#define __y(l) (h*l - y0)
#define __z(l) (h*l - z0)

static inline size_t get_initial_heap_size(int width, int height, int depth) {
  return static_cast<size_t>(fmax(8.0, log(width*height*depth)));
}

template <class base, class node, int num_neighbors>
marcher_3d<base, node, num_neighbors>::marcher_3d() {}

template <class base, class node, int num_neighbors>
marcher_3d<base, node, num_neighbors>::marcher_3d(int height, int width, int depth, double h,
                                   no_speed_func_t const &):
  abstract_marcher {static_cast<int>(get_initial_heap_size(width, height, depth))},
  _nodes {new node[width*height*depth]},
  _s_cache {new double[width*height*depth]},
  _h {h},
  _height {height},
  _width {width},
  _depth {depth}
{
  init();
}

template <class base, class node, int num_neighbors>
marcher_3d<base, node, num_neighbors>::marcher_3d(
  int height, int width, int depth, double h,
  std::function<double(double, double, double)> s,
  double x0, double y0, double z0):
  abstract_marcher {static_cast<int>(get_initial_heap_size(width, height, depth))},
  _nodes {new node[width*height*depth]},
  _s_cache {new double[width*height*depth]},
  _h {h},
  _height {height},
  _width {width},
  _depth {depth}
{
  // Grab a writable pointer to cache the speed function values.
  double x, z, * ptr = const_cast<double *>(_s_cache);
  for (int k = 0; k < depth; ++k) {
    z = __z(k);
    for (int j = 0; j < width; ++j) {
      x = __x(j);
      for (int i = 0; i < height; ++i) {
        ptr[linear_index(i, j, k)] = s(x, __y(i), z);
      }
    }
  }

  init();
}

template <class base, class node, int num_neighbors>
marcher_3d<base, node, num_neighbors>::marcher_3d(int height, int width, int depth, double h,
                                   double const * s_cache):
  abstract_marcher {static_cast<int>(get_initial_heap_size(width, height, depth))},
  _nodes {new node[width*height*depth]},
  _s_cache {new double[width*height*depth]},
  _h {h},
  _height {height},
  _width {width},
  _depth {depth}
{
  memcpy((void *) _s_cache, (void *) s_cache,
         sizeof(double)*height*width*depth);
  init();
}

template <class base, class node, int num_neighbors>
marcher_3d<base, node, num_neighbors>::~marcher_3d()
{
  assert(_nodes != nullptr);
  delete[] _nodes;

  assert(_s_cache != nullptr);
  delete[] _s_cache;
}

template <class base, class node, int num_neighbors>
void marcher_3d<base, node, num_neighbors>::add_boundary_node(
  int i, int j, int k, double value)
{
  assert(operator()(i, j, k).is_far() || operator()(i, j, k).is_valid());
  if (operator()(i, j, k).is_valid()) return;
  assert(in_bounds(i, j, k));
  visit_neighbors(&(operator()(i, j, k) = {i, j, k, value}));
}

#define LINE(p0, u0, s, s0, h)                                  \
  updates::line<base::F_>()(norm2<3>(p0), u0, s, s0, h)

template <class base, class node, int num_neighbors>
void
marcher_3d<base, node, num_neighbors>::add_boundary_node(
  double x, double y, double z, double s, double value)
{
  double h = get_h(), i = y/h, j = x/h, k = z/h, u0 = value, s0 = s;
  assert(in_bounds(static_cast<int>(i), static_cast<int>(j), static_cast<int>(k)));

  // TODO: make this more general: see comment in marcher.impl.hpp for
  // related function
  int is[2] = {(int) floor(i), (int) floor(i) + 1};
  int js[2] = {(int) floor(j), (int) floor(j) + 1};
  int ks[2] = {(int) floor(k), (int) floor(k) + 1};

  double ps[8][3] = {
    {i - is[0], j - js[0], k - ks[0]},
    {i - is[1], j - js[0], k - ks[0]},
    {i - is[0], j - js[1], k - ks[0]},
    {i - is[1], j - js[1], k - ks[0]},
    {i - is[0], j - js[0], k - ks[1]},
    {i - is[1], j - js[0], k - ks[1]},
    {i - is[0], j - js[1], k - ks[1]},
    {i - is[1], j - js[1], k - ks[1]},
  };

  for (int a = 0; a < 8; ++a) {
    int b0 = a & 1, b1 = (a & 2) >> 1, b2 = (a & 4) >> 2;
    int i_ = is[b0], j_ = js[b1], k_ = ks[b2];
    assert(in_bounds(i_, j_, k_));
    assert(operator()(i_, j_, k_).is_far());
    double s_hat = get_speed(i_, j_, k_);
    double u_hat = LINE(ps[a], u0, s_hat, s0, h);
    insert_into_heap(
      &(operator()(i_, j_, k_) = {i_, j_, k_, u_hat, state::trial}));
  }
}

#undef LINE

template <class base, class node, int num_neighbors>
void
marcher_3d<base, node, num_neighbors>::add_boundary_nodes(
  node const * nodes, int num)
{
  auto tmp = static_cast<node const **>(malloc(sizeof(void *)*num));
  for (int i = 0; i < num; ++i) {
    tmp[i] = &nodes[i];
  }
  add_boundary_nodes(tmp, num);
  free(tmp);
}

template <class base, class node, int num_neighbors>
void
marcher_3d<base, node, num_neighbors>::add_boundary_nodes(
  node const* const* nodes, int num)
{
  for (int l = 0; l < num; ++l) {
    auto n = nodes[l];
    int i = n->get_i(), j = n->get_j(), k = n->get_k();
    assert(in_bounds(i, j, k));
    assert(operator()(i, j, k).is_far());
    double u = n->get_value();
    insert_into_heap(&(operator()(i, j, k) = {i, j, k, u, state::trial}));
  }
}

template <class base, class node, int num_neighbors>
void marcher_3d<base, node, num_neighbors>::set_node_fac_center(
  int i, int j, int k, typename node::fac_center const * fc)
{
#if EIKONAL_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(i, j, k));
  assert(in_bounds(fc->i, fc->j, fc->k));
#endif
  operator()(i, j, k).set_fac_center(fc);
}

template <class base, class node, int num_neighbors>
double marcher_3d<base, node, num_neighbors>::get_value(int i, int j, int k) const {
#if EIKONAL_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(i, j, k));
#endif
  return operator()(i, j, k).get_value();
}

template <class base, class node, int num_neighbors>
node & marcher_3d<base, node, num_neighbors>::operator()(int i, int j, int k) {
#if EIKONAL_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(i, j, k));
  assert(_nodes != nullptr);
#endif
  return _nodes[linear_index(i, j, k)];
}

template <class base, class node, int num_neighbors>
node const & marcher_3d<base, node, num_neighbors>::operator()(int i, int j, int k) const {
#if EIKONAL_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(i, j, k));
  assert(_nodes != nullptr);
#endif
  return _nodes[linear_index(i, j, k)];
}

template <class base, class node, int num_neighbors>
bool marcher_3d<base, node, num_neighbors>::in_bounds(int i, int j, int k) const {
  return (unsigned) i < (unsigned) _height &&
    (unsigned) j < (unsigned) _width && (unsigned) k < (unsigned) _depth;
}

template <class base, class node, int num_neighbors>
double marcher_3d<base, node, num_neighbors>::get_speed(int i, int j, int k) const {
#if EIKONAL_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(i, j, k));
  assert(_s_cache != nullptr);
#endif
  return _s_cache[linear_index(i, j, k)];
}

template <class base, class node, int num_neighbors>
bool marcher_3d<base, node, num_neighbors>::is_valid(int i, int j, int k) const {
  return in_bounds(i, j, k) && operator()(i, j, k).is_valid();
}

template <class base, class node, int num_neighbors>
void marcher_3d<base, node, num_neighbors>::init() {
  for (int i = 0; i < _height; ++i) {
    for (int j = 0; j < _width; ++j) {
      for (int k = 0; k < _depth; ++k) {
        operator()(i, j, k).set_i(i);
        operator()(i, j, k).set_j(j);
        operator()(i, j, k).set_k(k);
      }
    }
  }
}

#define __maxabs3(x, y, z) \
  std::max(std::abs(x), std::max(std::abs(y), std::abs(z)))

template <class base, class node, int num_neighbors>
void marcher_3d<base, node, num_neighbors>::visit_neighbors_impl(abstract_node * n) {
  int i = static_cast<node *>(n)->get_i();
  int j = static_cast<node *>(n)->get_j();
  int k = static_cast<node *>(n)->get_k();

  // See comments in marcher.impl.hpp; the visit_neighbors_impl there
  // is done analogously to this one.

  int a, b, c;

  // Stage neighbors.
  for (int l = 0; l < num_neighbors; ++l) {
    a = i + __di(l), b = j + __dj(l), c = k + __dk(l);
    if (in_bounds(a, b, c) && operator()(a, b, c).is_far()) {
      operator()(a, b, c).set_trial();
      insert_into_heap(&operator()(a, b, c));
    }
  }

  // Get valid neighbors.
  node * valid_nb[26], * child_nb[num_neighbors];
  memset(valid_nb, 0x0, 26*sizeof(abstract_node *));
  for (int l = 0; l < 26; ++l) {
    a = i + __di(l), b = j + __dj(l), c = k + __dk(l);
    if (in_bounds(a, b, c) && operator()(a, b, c).is_valid()) {
      valid_nb[l] = &this->operator()(a, b, c);
    }
  }

  int di_l, dj_l, dk_l;
  auto const set_child_nb = [&] (int parent) {
    memset(child_nb, 0x0, num_neighbors*sizeof(abstract_node *));
    child_nb[parent] = static_cast<node *>(n);
    for (int m = 0; m < num_neighbors; ++m) {
      if (m == parent) {
        continue;
      }
      int di_lm, dj_lm, dk_lm;
      if (std::abs(di_lm = di_l + __di(m)) > 1 ||
          std::abs(dj_lm = dj_l + __dj(m)) > 1 ||
          std::abs(dk_lm = dk_l + __dk(m)) > 1) {
        continue;
      }
      if (in_bounds(i + di_lm, j + dj_lm, k + dk_lm)) {
        int p = d2l(di_lm, dj_lm, dk_lm);
        child_nb[m] = valid_nb[p];
      }
    }
  };

  auto & s_hat = static_cast<base *>(this)->s_hat;
  auto const update = [&] (int i, int j, int k, int parent) {
    auto T = inf<double>;
    node * update_node = &operator()(i, j, k);
    s_hat = this->get_speed(i, j, k);
    update_impl(update_node, child_nb, parent, T);
#if NODE_MONITORING
    if (update_node->monitoring_node()) {
      std::cout << *update_node << std::endl;
    }
#endif
    if (T < update_node->get_value()) {
#if EIKONAL_DEBUG && !RELWITHDEBINFO
      assert(T >= 0);
#endif
      update_node->set_value(T);
      adjust_heap_entry(update_node);
    }
  };

  auto const get_parent = [] (int l) {
    // TODO: check base::num_nb to reduce amount of branching here
    if (l < 6) return (l + 3) % 6;
    else if (l < 18) return 22 - 2*(l/2) + (l % 2);
    else return 42 - 2*(l/2) + (l % 2);
  };

  for (int l = 0; l < num_neighbors; ++l) {
    if (!valid_nb[l]) {
      di_l = __di(l), dj_l = __dj(l), dk_l = __dk(l);
      a = i + di_l, b = j + dj_l, c = k + dk_l;
      if (!in_bounds(a, b, c)) continue;
      int parent = get_parent(l);
      set_child_nb(parent);
      update(a, b, c, parent);
    }
  }
}

#undef __maxabs3

#undef __di
#undef __dj
#undef __dk

#undef __x
#undef __y
#undef __z

#endif // __MARCHER_3D_IMPL_HPP__
