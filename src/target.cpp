#include <st/serial.h>
#include <st/target.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#define LOGN(str) std::cerr << str << std::endl
#define LOG(str) std::cerr << str;

auto disable_ulisp_echo(SerialPort& serial) -> void {
  // Works with a special build of ulisp, where auto re-enabling of echo is
  // compiled out.

  const std::string noecho{";noecho;"};
  serial.send(noecho);

  // flush serial buffer
  serial.null_route(100);
}

auto LispTarget::_init(SerialPort& port) -> void { disable_ulisp_echo(port); }

auto serial_read_line(SerialPort& serial, std::string& line) -> bool {
  // TODO: input error handling, using some C++ smarts
  while (true) {
    char c;
    int len = serial.getc(c);

    if (len == 0) {
      // better luck next time
      continue;
    }

    if (c == '\n') {
      break;
    }

    LOGN("got [" << c << "]");
    line += c;

    if (c == '>') {
      break;
    }
  }

  return true;
}

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

auto no_trail_whitespace(std::string& str) -> std::string_view {
  std::string_view view = str;

  while (std::isspace(view.back())) {
    view.remove_suffix(1);
  }

  return view;
}

auto is_prompt(std::string line) -> bool { return (no_trail_whitespace(line).back() == '>'); }

template <typename F> auto read_result(F read_line) -> std::string {
  // Read the result of the computation, until the lisp prompt
  // We can optionally track the memory consumption from the prompt
  char c;
  std::string result;
  std::string line;

  while (read_line(line) && !is_prompt(line)) {
    // do we need to give up after some time? e.g. if s-exp is bad, the prompt
    // will never be shown.

    result += line + '\n';
    line = "";
  }

  // Remove whitespace around result
  while (std::isspace(result.front())) {
    result.erase(0, 1);
  }

  // Remove whitespace around result
  while (std::isspace(result.back())) {
    result.pop_back();
  }

  return result;
}

auto LispTarget::evaluate(const std::string& input) -> std::string {
  _port.send(input);

  std::string result
      = read_result([this](std::string& line) -> bool { return serial_read_line(_port, line); });

  return result;
}
