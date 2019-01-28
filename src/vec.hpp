#pragma once

#include <algorithm>
#include <initializer_list>

#include <assert.h>
#include <math.h>

template <class T, int n>
struct vec
{
  vec() {}

  vec(std::initializer_list<T> ts) {
    std::copy(ts.begin(), ts.end(), _data);
  }

  inline T & operator[](int i) {
    return _data[i];
  }

  inline T const & operator[](int i) const {
    return _data[i];
  }

  inline vec<T, n> & operator*=(T const & t) {
    for (int i = 0; i < n; ++i) {
      _data[i] *= t;
    }
    return *this;
  }

  friend vec<T, n> operator*(vec<T, n> x, T const & t) {
    x *= t;
    return x;
  }

  friend vec<T, n> operator*(T const & t, vec<T, n> x) {
    x *= t;
    return x;
  }

  inline vec<T, n> & operator/=(T const & t) {
    for (int i = 0; i < n; ++i) {
      _data[i] /= t;
    }
    return *this;
  }

  friend vec<T, n> operator/(vec<T, n> x, T const & t) {
    x /= t;
    return x;
  }

  friend vec<T, n> operator/(T const & t, vec<T, n> x) {
    x /= t;
    return x;
  }

  friend T operator*(vec<T, n> const & x, vec<T, n> const & y) {
    T t {0};
    for (int i = 0; i < n; ++i) {
      t += x[i]*y[i];
    }
    return t;
  }

  inline vec<T, n> & operator+=(vec<T, n> const & x) {
    for (int i = 0; i < n; ++i) {
      _data[i] += x[i];
    }
    return *this;
  }

  friend vec<T, n> operator+(vec<T, n> x, vec<T, n> const & y) {
    x += y;
    return x;
  }

  inline vec<T, n> & operator-=(vec<T, n> const & x) {
    for (int i = 0; i < n; ++i) {
      _data[i] -= x[i];
    }
    return *this;
  }

  friend vec<T, n> operator-(vec<T, n> x, vec<T, n> const & y) {
    x -= y;
    return x;
  }

  inline T norm1() const {
    T t {0};
    for (int i = 0; i < n; ++i) {
      t += std::abs(_data[i]);
    }
    return t;
  }

  inline T norm2sq() const {
    T t {0};
    for (int i = 0; i < n; ++i) {
      t += _data[i]*_data[i];
    }
    return t;
  }

  inline T norm2() const {
    return sqrt(norm2sq());
  }

  inline T normi() const {
    T t {0};
    for (int i = 0; i < n; ++i) {
      t = std::max(t, std::abs(_data[i]));
    }
    return t;
  }

OLIM_PRIVATE:
  T _data[n];
};

template <class T, int n>
T dist1(vec<T, n> const & u, vec<T, n> const & v) {
  return (u - v).norm1();
}

template <class T, int n>
T dist2(vec<T, n> const & u, vec<T, n> const & v) {
  return (u - v).norm2();
}

template <class T, int n>
T dist2sq(vec<T, n> const & u, vec<T, n> const & v) {
  return (u - v).norm2sq();
}

template <class T, int n>
T disti(vec<T, n> const & u, vec<T, n> const & v) {
  return (u - v).normi();
}

/**
 * Number of bits that are set for integers 0 through 7.
 */
constexpr int nbits[8] = {0, 1, 1, 2, 1, 2, 2, 3};

/**
 * Compute dot product between p and q, interpreting them as bit
 * vectors. Assumes that 0 <= p < 8 and 0 <= q < 8.
 */
constexpr int dot(int p, int q) {
  assert(0 <= p && p < 8);
  assert(0 <= q && q < 8);
  return nbits[p & q];
}
