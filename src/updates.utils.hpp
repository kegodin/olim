#ifndef __UPDATE_RULES_UTILS_HPP__
#define __UPDATE_RULES_UTILS_HPP__

#include <cassert>
#include <cmath>
#include <utility>

template <int dim>
void sub(double const * p, double const * q, double * r);

template <>
inline void sub<2>(double const * p, double const * q, double * r) {
  r[0] = p[0] - q[0];
  r[1] = p[1] - q[1];
}

template <>
inline void sub<3>(double const * p, double const * q, double * r) {
  r[0] = p[0] - q[0];
  r[1] = p[1] - q[1];
  r[2] = p[2] - q[2];
}

template <int dim>
void axpy(double alpha, double const * x, double const * y, double * z);

template <>
inline void
axpy<2>(double alpha, double const * x, double const * y, double * z) {
  z[0] = alpha*x[0] + y[0];
  z[1] = alpha*x[1] + y[1];
}

template <>
inline void
axpy<3>(double alpha, double const * x, double const * y, double * z) {
  z[0] = alpha*x[0] + y[0];
  z[1] = alpha*x[1] + y[1];
  z[2] = alpha*x[2] + y[2];
}

template <int dim>
double dot(double const * x, double const * y);

template <>
inline double dot<2>(double const * x, double const * y) {
  return x[0]*y[0] + x[1]*y[1];
}

template <>
inline double dot<3>(double const * x, double const * y) {
  return x[0]*y[0] + x[1]*y[1] + x[2]*y[2];
}

template <int dim>
void scal_inplace(double alpha, double * x);

template <>
inline void
scal_inplace<2>(double alpha, double * x) {
  x[0] *= alpha;
  x[1] *= alpha;
}

template <>
inline void
scal_inplace<3>(double alpha, double * x) {
  x[0] *= alpha;
  x[1] *= alpha;
  x[2] *= alpha;
}

template <int dim>
double dist1(double const * p, double const * q);

template <>
inline double dist1<3>(double const * p, double const * q) {
  return
    fabs(q[0] - p[0]) +
    fabs(q[1] - p[1]) +
    fabs(q[2] - p[2]);
}

template <int dim>
double dist2(double const * p, double const * q);

template <>
inline double dist2<2>(double const * p, double const * q) {
  return std::sqrt(
    (q[0] - p[0])*(q[0] - p[0]) +
    (q[1] - p[1])*(q[1] - p[1]));
}

template <>
inline double dist2<3>(double const * p, double const * q) {
  return std::sqrt(
    (q[0] - p[0])*(q[0] - p[0]) +
    (q[1] - p[1])*(q[1] - p[1]) +
    (q[2] - p[2])*(q[2] - p[2]));
}

template <int dim>
double dist2sq(double const * p, double const * q);

template <>
inline double dist2sq<3>(double const * p, double const * q) {
  return
    (q[0] - p[0])*(q[0] - p[0]) +
    (q[1] - p[1])*(q[1] - p[1]) +
    (q[2] - p[2])*(q[2] - p[2]);
}

template <int dim>
double distmax(double const * p, double const * q);

template <>
inline double distmax<3>(double const * p, double const * q) {
  return fmax(
    fabs(q[0] - p[0]),
    fmax(
      fabs(q[1] - p[1]),
      fabs(q[2] - p[2])));
}

template <int dim>
double norm2(double const * p);

template <>
inline double norm2<2>(double const * p) {
  return std::sqrt(p[0]*p[0] + p[1]*p[1]);
}

template <>
inline double norm2<3>(double const * p) {
  return std::sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
}

template <char c>
using ffvec = std::integral_constant<char, c>;

/**
 * Number of bits that are set for integers 0 through 7.
 */
constexpr char nbits[8] = {0, 1, 1, 2, 1, 2, 2, 3};

/**
 * Gets the ith component of the bit vector p.
 */
constexpr char component(char p, char i) {
  // TODO: handle signedness of p
  return (p & 1 << i) >> i;
}

/**
 * Computes Hamming norm of p, interpreted as a bit vector. Assumes
 * that 0 <= p < 8. Implicitly assumes the dimension of the vector is
 * between 1 and 3. For vectors with dimension 1 or 2, make sure the
 * more significant bits are set to zero (i.e., valid vectors of
 * dimension 1 and 2 are 0, 1, 2, and 3).
 */
constexpr char weight(char p) {
  assert(0 <= p && p < 8);
  return nbits[static_cast<int>(p)];
}

/**
 * Compute dot product between p and q, interpreting them as bit
 * vectors. Assumes that 0 <= p < 8 and 0 <= q < 8.
 */
constexpr char dot(char p, char q) {
  assert(0 <= p && p < 8);
  assert(0 <= q && q < 8);
  return nbits[p & q];
}

/**
 * Project coordinates (lam1, lam2) onto nearest barycentric
 * coordinates; i.e., orthogonally project (lam1, lam2) onto the
 * simplex {(x, y): x >= 0, y >= 0, x + y <= 1}.
 */
inline void proj_bary(double & lam1, double & lam2) {
  double const sum = lam1 + lam2;
  if (sum > 1) {
    double const shift = (1 - sum)/2;
    lam1 += shift;
    lam2 += shift;
  }
  lam1 = lam1 < 0 ? 0 : (lam1 > 1 ? 1 : lam1);
  lam2 = lam2 < 0 ? 0 : (lam2 > 1 ? 1 : lam2);
}

#endif // __UPDATE_RULES_UTILS_HPP__