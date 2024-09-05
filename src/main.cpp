#include <sexp.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <thread>
#include <chrono>

auto is_prompt(std::string line) -> bool { return (line.back() == '>'); }

auto stream_read_line(std::istream& is, std::string& line) -> bool {
  // Poll for input
  // TODO: add a timeout or sprinkle some async
  // and use `return false`
  is >> std::noskipws;  // Don't skip whitespace

  while (!getline(is, line)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return true;
}

template<typename F>
auto read_result(F read_line) -> std::string {
  // Read the result of the computation, until the lisp prompt
  // We can optionally track the memory consumption from the prompt
  char c;
  std::string result;
  std::string line;

  while (read_line(line) && !is_prompt(line)) {
    // do we need to give up after some time? e.g. if s-exp is bad, the prompt
    // will never be shown.
    // std::cerr << "[" << line << "]";

    result += line + '\n';
  }

  // std::cerr << std::endl;

  return result;
}

auto main(int argc, char* argv[]) -> int {
  assert(argc == 2);

  std::string uart_path = argv[1];
  std::cerr << "Path: " << uart_path << std::endl;
  std::cerr.flush();

  // will be flushed/closed by RAII
  std::fstream uartstream(uart_path);
  assert(uartstream.is_open());

  Sexp exp;
  std::stringstream is("(+ 1 2 (* 3 4))");
  is >> exp;

  std::cerr << "Eval: " << exp << std::endl;
  // "send" s-exp to target
  // for now, we echo it back to the terminal, to avoid needing a sink program
  std::cout << exp << std::endl;

  // read and log result
  std::string result = read_result(
      [&uartstream](std::string& line) -> bool { return stream_read_line(uartstream, line); });
  std::cerr << "Result: " << result;

  return 0;
}
