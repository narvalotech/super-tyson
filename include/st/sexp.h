#ifndef SEXP_H_
#define SEXP_H_

#include <iostream>

class Sexp {
public:
  // We use the default constructor.

  friend auto operator>>(std::istream& is, Sexp& obj) -> std::istream&;

  auto get() const -> std::string const { return exp; }

private:
  // since we're not building an interpreter we don't actually need to
  // parse the S-exp into an syntax tree. Storing the string is good
  // enough for passing to the target interpreter.
  std::string exp;
};

auto operator<<(std::ostream& os, const Sexp& obj) -> std::ostream&;
auto operator>>(std::istream& stream, Sexp& obj) -> std::istream&;

#endif // SEXP_H_
