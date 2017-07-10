#define BOOST_TEST_MODULE olim4_rhr_lut

#include <boost/test/included/unit_test.hpp>

#include "olim4_rhr_lut.hpp"
#include "speed_funcs.hpp"

BOOST_AUTO_TEST_CASE (trivial_case_works) {
  olim4_rhr_lut m {1, 1};
  m.add_boundary_node(0, 0);
  m.run();
  BOOST_CHECK_EQUAL(m.get_value(0, 0), 0.0);
}

BOOST_AUTO_TEST_CASE (adjacent_update_works) {
  olim4_rhr_lut m {2, 1, 0.5};
  m.add_boundary_node(0, 0);
  m.run();
  BOOST_CHECK_EQUAL(m.get_value(1, 0), 0.5);
}

BOOST_AUTO_TEST_CASE (neighboring_values_are_correct) {
  olim4_rhr_lut m {3, 3, 1};
  m.add_boundary_node(1, 1);
  m.run();
  double gt[] = {
    1.707106781186547,
    1,
    1.707106781186547,
    1,
    0,
    1,
    1.707106781186547,
    1,
    1.707106781186547
  };
  int k = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      BOOST_TEST(
        gt[k++] == m.get_value(i, j),
        boost::test_tools::tolerance(1e-15));
    }
  }
}

BOOST_AUTO_TEST_CASE (masha_s_values_are_correct) {
  double gt[] = {
    0.02939463262323,
    0.01540058973304,
    0.02037282789484,
    0.01089789836732,
    2.030579675453e-07,
    0.01070992017687,
    0.02286521936613,
    0.01465429342172,
    0.02703978500521,
  };

  olim4_rhr_lut m {21, 21, 0.1, s7, 1, 1};
  m.add_boundary_node(10, 10, 2.030579675453e-07);
  m.run();

  int k = 0;
  auto const tol = boost::test_tools::tolerance(1e-4);
  for (int i = 9; i <= 11; ++i) {
    for (int j = 9; j <= 11; ++j) {
      BOOST_TEST(gt[k++] == m.get_value(i, j), tol);
    }
  }

  for (int i = 0; i < 21; ++i) {
    for (int j = 0; j < 21; ++j) {
      std::cout << m.get_value(i, j) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE (masha_small_test) {
  double gt[] = {
    0.02939463262323,
    0.01540058973304,
    0.02037282789484,
    0.01089789836732,
    2.030579675453e-07,
    0.01070992017687,
    0.02286521936613,
    0.01465429342172,
    0.02703978500521,
  };
  double h = 0.1, x0 = 0.1, y0 = 0.1;
  olim4_rhr_lut m {3, 3, h, s7, x0, y0};
  m.add_boundary_node(1, 1, 2.030579675453e-07);
  m.run();
  for (int i = 0, k = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      BOOST_TEST(m.get_value(i, j) == gt[k++],
                 boost::test_tools::tolerance(1e-5));
    }
  }
}

BOOST_AUTO_TEST_CASE (rectangular_domain_works) {
  double h = 0.1, x0 = 1, y0 = 1;
  olim4_rhr_lut m {11, 21, h, default_speed_func, x0, y0};
  m.add_boundary_node(5, 10);
  m.run();
  BOOST_TEST(m.get_value(0, 0) == 1.17825, boost::test_tools::tolerance(1e-5));
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// End: