#include <olim>

#include <iostream>
#include <string>

template <class marcher_3d>
void run_marcher_3d(int n) {
  double h = 2./(n - 1);
  int i = n/2;
  marcher_3d m {{n, n, n}, h};
  m.add_src({i, i, i});
  m.run();
}

int main(int argc, char * argv[]) {
  if (argc != 3) {
    std::cout << "usage: " << argv[0] << " marcher N" << std::endl;
    std::exit(1);
  }

  std::string marcher_name {argv[1]};
  int n = std::stoi(argv[2]);

  if (marcher_name == "basic_marcher_3d") run_marcher_3d<fmm<3>>(n);

  if (marcher_name == "olim6_mp0") run_marcher_3d<olim6_mp0>(n);
  if (marcher_name == "olim6_mp1") run_marcher_3d<olim6_mp1>(n);
  if (marcher_name == "olim6_rhr") run_marcher_3d<olim6_rhr>(n);

  if (marcher_name == "olim18_mp0") run_marcher_3d<olim18_mp0>(n);
  if (marcher_name == "olim18_mp1") run_marcher_3d<olim18_mp1>(n);
  if (marcher_name == "olim18_rhr") run_marcher_3d<olim18_rhr>(n);

  if (marcher_name == "olim26_mp0") run_marcher_3d<olim26_mp0>(n);
  if (marcher_name == "olim26_mp1") run_marcher_3d<olim26_mp1>(n);
  if (marcher_name == "olim26_rhr") run_marcher_3d<olim26_rhr>(n);

  if (marcher_name == "olim3d_mp0") run_marcher_3d<olim3d_mp0>(n);
  if (marcher_name == "olim3d_mp1") run_marcher_3d<olim3d_mp1>(n);
  if (marcher_name == "olim3d_rhr") run_marcher_3d<olim3d_rhr>(n);
}