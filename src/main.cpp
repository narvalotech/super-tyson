#include <st/sexp.h>
#include <st/serial.h>
#include <st/linuxserial.h>

#include <iostream>
#include <fstream>
#include <cassert>
#include <thread>
#include <chrono>

#define LOGN(str) std::cerr << str << std::endl
#define LOG(str) std::cerr << str;

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

  // LOGN("line: " << line);

  return true;
}

auto no_trail_whitespace(std::string& str) -> std::string_view {
  std::string_view view = str;

  while (std::isspace(view.back())) {
    view.remove_suffix(1);
  }

  return view;
}

auto is_prompt(std::string line) -> bool {
  return (no_trail_whitespace(line).back() == '>');
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

    result += line + '\n';
    line = "";
  }

  // std::cerr << std::endl;

  return result;
}

auto disable_ulisp_echo(SerialPort& serial) -> void {
  // Works with a special build of ulisp, where auto re-enabling of echo is
  // compiled out.

  const std::string noecho {";noecho;"};
  serial.send(noecho);

  // flush serial buffer
  serial.null_route(100);
}

auto main(int argc, char* argv[]) -> int {
  assert(argc == 2);

  std::string uart_path = argv[1];
  std::cerr << "Path: " << uart_path << std::endl;
  std::cerr.flush();

  // TODO: use argv[1] to select serial port type
  // TODO: pass serial params via cli too
  LinuxSerialPort serial_port(uart_path.c_str(), 115200, false);

  disable_ulisp_echo(serial_port);

  Sexp exp;
  std::cin >> exp;

  std::cerr << "Eval: " << exp << std::endl;
  // send s-exp to target
  serial_port.send(exp.get());

  // read and log result

  std::string result = read_result(
      [&serial_port](std::string& line) -> bool { return serial_read_line(serial_port, line); });

  std::cerr << "Result: {" << result << "}";

  return 0;
}
