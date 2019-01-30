#pragma once

#include <assert.h>
#include <math.h>

#include "common.hpp"
#include "offsets.hpp"
#include "updates.line.hpp"

// TODO: really need an external memory constructor that will let us
// use external memory somewhere for _s_cache and _U so we don't have
// to double up on these...

template <class base, int num_nb>
marcher<base, num_nb>::marcher(vec2<int> dims, double h, no_slow_t const &):
  _size {dims.product()},
  _heap {{this}, initial_heap_capacity(_size)},
  _U {new double[_size]},
  _s_cache {new double[_size]},
  _state {new state[_size]},
  _heap_pos {new int[_size]},
  _h {h},
  _dims {dims}
{
  init();
}

template <class base, int num_nb>
marcher<base, num_nb>::marcher(vec2<int> dims, double h, double const * s_cache):
  _size {dims.product()},
  _heap {{this}, initial_heap_capacity(_size)},
  _U {new double[_size]},
  _s_cache {new double[_size]},
  _state {new state[_size]},
  _heap_pos {new int[_size]},
  _h {h},
  _dims {dims}
{
  init();

  memcpy((void *) _s_cache, (void *) s_cache, sizeof(double)*_size);
}

template <class base, int num_nb>
marcher<base, num_nb>::marcher(vec2<int> dims, double h,
                               std::function<double(vec2<double>)> s,
                               vec2<double> origin):
  _size {dims.product()},
  _heap {{this}, initial_heap_capacity(_size)},
  _U {new double[_size]},
  _s_cache {new double[_size]},
  _state {new state[_size]},
  _heap_pos {new int[_size]},
  _h {h},
  _dims {dims}
{
  init();

  double * ptr = const_cast<double *>(_s_cache);
  for (int i = 0; i < dims[1]; ++i) {
    for (int j = 0; j < dims[0]; ++j) {
      ptr[to_linear_index({i, j})] = s(vec2<double> {h*i, h*j} - origin);
    }
  }
}

template <class base, int num_nb>
marcher<base, num_nb>::~marcher()
{
  delete[] _U;
  delete[] _s_cache;
  delete[] _state;
  delete[] _heap_pos;
}

template <class base, int num_nb>
void marcher<base, num_nb>::init()
{
  for (int i = 0; i < _size; ++i) {
    _U[i] = inf<double>;
  }

  for (int i = 0; i < _size; ++i) {
    _state[i] = state::far;
  }
}

template <class base, int num_nb>
void marcher<base, num_nb>::run()
{
  while (!_heap.empty()) {
    int lin = _heap.front();
    _heap.pop_front();
    _state[lin] = state::valid;
    visit_neighbors(lin);
  }  
}

template <class base, int num_nb>
void
marcher<base, num_nb>::add_boundary_node(vec2<int> inds, double value)
{
#if OLIM_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(inds));
#endif
  int lin = to_linear_index(inds);
  _U[lin] = value;
  _state[lin] = state::trial;
  _heap.insert(lin);
}

template <class base, int num_nb>
void
marcher<base, num_nb>::add_boundary_nodes(
  vec2<int> const * inds, double const * U, int num)
{
  for (int i = 0; i < num; ++i) {
    add_boundary_node(inds[i], U[i]);
  }
}

template <class base, int num_nb>
void
marcher<base, num_nb>::add_boundary_nodes(
  std::tuple<int, int, double> const * nodes, int num)
{
  for (int k = 0; k < num; ++k) {
    add_boundary_node(
      std::get<0>(nodes[k]),
      std::get<1>(nodes[k]),
      std::get<2>(nodes[k]));
  }
}

template <class base, int num_nb>
void
marcher<base, num_nb>::add_boundary_node(vec2<double> coords, double s, double value)
{
  double h = get_h(), u0 = value, s0 = s;
  vec2<double> inds = coords/h;
  assert(in_bounds(inds));

  double i = inds[0], j = inds[1];

  // TODO: this isn't as general as it could be. We also want to
  // handle cases where i or j are grid-aligned, in which case we need
  // to process 6 nodes. If i and j are both grid aligned, then we
  // should just call the integer add_boundary_node.
  int is[2] = {(int) floor(i), (int) floor(i) + 1};
  int js[2] = {(int) floor(j), (int) floor(j) + 1};

  vec2<double> P[4] = {
    {i - is[0], j - js[0]},
    {i - is[1], j - js[0]},
    {i - is[0], j - js[1]},
    {i - is[1], j - js[1]}
  };

  for (int a = 0; a < 4; ++a) {
    int b0 = a & 1, b1 = (a & 2) >> 1;
    int i_ = is[b0], j_ = js[b1];

    assert(in_bounds({i_, j_}));

    int lin = to_linear_index({i_, j_});
    _U[lin] = updates::line<base::F_>()(P[a].norm2(), u0, _s_cache[lin], s0, h);
    _state[lin] = state::trial;
    _heap.insert(lin);
  }
}

template <class base, int num_nb>
void
marcher<base, num_nb>::set_fac_src(vec2<int> inds, fac_src const * fc)
{
#if OLIM_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(inds));
#endif
  _lin2fac[to_linear_index(inds)] = fc;
}

template <class base, int num_nb>
double
marcher<base, num_nb>::get_value(vec2<int> inds) const
{
#if OLIM_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(inds));
#endif
  return _U[to_linear_index(inds)];
}

template <class base, int num_nb>
bool
marcher<base, num_nb>::in_bounds(vec2<int> inds) const
{
  return (unsigned) inds[0] < (unsigned) _dims[0]
      && (unsigned) inds[1] < (unsigned) _dims[1];
}

template <class base, int num_nb>
double
marcher<base, num_nb>::get_s(vec2<int> inds) const
{
#if OLIM_DEBUG && !RELWITHDEBINFO
  assert(in_bounds(inds));
  assert(_s_cache != nullptr);
#endif
  return _s_cache[to_linear_index(inds)];
}

// TODO: we want to delete this---right now, it's a bit muddled, since
// it should probably be called `is_inbounds_and_valid'... but we want
// to get rid of `in_bounds', anyway, so once we do that, this
// function will just be deleted
template <class base, int num_nb>
bool
marcher<base, num_nb>::is_valid(vec2<int> inds) const
{
  return in_bounds(inds) && _state[to_linear_index(inds)] == state::valid;
}

template <class base, int num_nb>
void
marcher<base, num_nb>::visit_neighbors(int lin_center)
{
  vec2<int> inds = to_vector_index(lin_center);

  // Traverse the update neighborhood of n and set all far nodes to
  // trial and insert them into the heap.
  for (int i = 0; i < num_nb; ++i) {
    vec2<int> inds_ = inds + get_offset<2>(i);
    int lin = to_linear_index(inds_);
    if (in_bounds(inds_) && _state[lin] == state::far) {
      _state[lin] = state::trial;
      _heap.insert(lin);
    }
  }

  // Find the valid neighbors in the "full" neighborhood of n
  // (i.e. the unit max norm ball).
  for (int i = 0; i < 8; ++i) {
    valid_nb[i] = -1;
    vec2<int> inds_ = inds + get_offset<2>(i);
    int lin = to_linear_index(inds_);
    if (in_bounds(inds_) && _state[lin] == state::valid) {
      valid_nb[i] = lin;
    }
  }

  // Some explanation of the indices used below:
  // - l is a radial index circling (a, b)
  // - parent is the radial index of (i, j) expressed in the same
  //   index space as l
  auto const set_nb = [&] (int parent, vec2<int> offset) {
    int * nb = static_cast<base *>(this)->nb;
    for (int l = 0; l < num_nb; ++l) {
      nb[l] = -1;
    }
    nb[parent] = lin_center;
    for (int l = 0; l < num_nb; ++l) {
      if (l == parent) {
        continue;
      }
      vec2<int> offset_ = offset + get_offset<2>(l);
      if (offset_.normi() > 1) {
        continue;
      }
      if (in_bounds(inds + offset_)) {
        nb[l] = valid_nb[get_linear_offset<2>(offset_)];
      }
    }
  };

  // Update the node at (i, j). Before calling, `nb' needs to be
  // filled appropriately. Upon updating, this sets the value of n and
  // adjusts its position in the heap.
  auto const update = [&] (int lin_hat) {
    auto U = inf<double>;
    static_cast<base *>(this)->s_hat = _s_cache[lin_hat];
    update_impl(lin_hat, U);
    if (U < _U[lin_hat]) {
#if OLIM_DEBUG && !RELWITHDEBINFO
      assert(U >= 0);
#endif
      _U[lin_hat] = U;
      _heap.update(lin_hat);
    }
  };

  // Get the parent index of a radial index `i'.
  auto const get_parent = [] (int i) {
    // TODO: check base::num_nb to reduce amount of branching here
    if (num_nb == 4) {
      return (i + 2) % 4;
    } else {
      if (i < 4) return (i + 2) % 4;
      else return ((i - 2) % 4) + 4;
    }
  };

  // This is the main update loop. Each neighbor of n which isn't
  // valid is now trial. For each neighboring trial node, use
  // `set_nb' to grab its valid neighbors and use `update' to
  // actually update the node's value and update its position in the
  // heap.
  for (int i = 0; i < num_nb; ++i) {
    if (valid_nb[i] == -1) {
      vec2<int> offset = get_offset<2>(i);
      vec2<int> inds_ = inds + offset;
      if (!in_bounds(inds_)) continue;
      int parent = get_parent(i);
      set_nb(parent, offset);
      update(to_linear_index(inds_));
    }
  }
}
