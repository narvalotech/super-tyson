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

auto operator<<(std::ostream& os, const Sexp& obj) -> std::ostream& { return os << obj.get(); }

auto operator>>(std::istream& stream, Sexp& obj) -> std::istream& {
  // TODO: allow (and strip+keep) inline comments

  char c;
  auto nesting{0};
  std::string exp;

  stream >> std::noskipws;  // Don't skip whitespace

  if (stream >> c && c == '(') {
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

auto main() -> int {
  Sexp exp;
  std::cin >> exp;

  std::cout << "Read: " << exp << std::endl;

  return 0;
}
