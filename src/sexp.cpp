#include <st/sexp.h>
#include <iostream>

auto operator<<(std::ostream& os, const Sexp& obj) -> std::ostream& { return os << obj.get(); }

auto operator>>(std::istream& stream, Sexp& obj) -> std::istream& {
  // TODO: allow (and strip+keep) inline comments

  char c = 0;
  auto nesting{0};
  std::string exp;

  stream >> std::noskipws;  // Don't skip whitespace

  while (stream >> c && c != '(') {
    // Wait until the S-exp starts
  }

  if (c == '(') {
    nesting = 1;  // begin s-expression
    exp += c;

    // consume chars, taking into account the level of () nesting.
    while (stream >> c && nesting != 0) {
      exp += c;

      switch (c) {
        case '(':
          nesting++;
          break;
        case ')':
          nesting--;
          break;
      }
    }
  } else {
    // TODO: return failure properly
    stream.setstate(std::ios_base::failbit);
  }

  obj.exp = exp;

  return stream;
}
