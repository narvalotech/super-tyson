#include <sexp.h>
#include <iostream>

auto main() -> int {
  Sexp exp;
  std::cin >> exp;

  std::cout << "Read: " << exp << std::endl;

  return 0;
}
