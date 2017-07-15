#ifndef __FAST_MARCHER_3D_HPP__
#define __FAST_MARCHER_3D_HPP__

#include "heap.hpp"
#include "node_3d.hpp"
#include "speed_funcs.hpp"
#include "typedefs.h"

struct fast_marcher_3d
{
  void add_boundary_node(int i, int j, int k, double value = 0.0);
  void run();
  double get_value(int i, int j, int k) const;

protected:
  node_3d & operator()(int i, int j, int k);
  node_3d const & operator()(int i, int j, int k) const;
  void update_node_value(int i, int j, int k);
  void stage_neighbors(int i, int j, int k);
  void stage_neighbor(int i, int j, int k);
  bool in_bounds(int i, int j, int k) const;
  bool is_valid(int i, int j, int k) const;
  node_3d * get_next_node();
  double get_h() const;
  double S(int i, int j, int k);
  void adjust_heap_entry(node_3d * n);
  void insert_into_heap(node_3d * n);

private:
  virtual void update_node_value_impl(int i, int j, int k, double & T) = 0;
  virtual void stage_neighbors_impl(int i, int j, int k) = 0;

  node_3d * _nodes;
  heap<node_3d> _heap;
  double _h;
  double * _S_cache;
  speed_func_3d _S {default_speed_func_3d};
  double _x0 {0}, _y0 {0}, _z0 {0};
  int _height, _width, _depth;
};

#endif // __FAST_MARCHER_3D_HPP__

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
