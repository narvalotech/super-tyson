#include <st/linuxserial.h>
#include <st/sexp.h>
#include <st/target.h>

#include <cassert>
#include <iostream>

auto main(int argc, char* argv[]) -> int {
  assert(argc == 2);

  std::string uart_path = argv[1];
  std::cerr << "Path: " << uart_path << std::endl;
  std::cerr.flush();

  // TODO: use argv[1] to select serial port type
  // TODO: pass serial params via cli too
  LinuxSerialPort serial_port(uart_path.c_str(), 115200, false);
  LispTarget target(serial_port);

  Sexp exp;
  std::cin >> exp;

  std::cerr << "Eval: " << exp << std::endl;

  // Send to lisp super-computer
  std::string result = target.evaluate(exp.get());

  std::cerr << "Result: {" << result << "}";
  std::cout << result;

  return 0;
}
