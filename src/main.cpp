#include <st/linuxserial.h>
#include <st/log.h>
#include <st/sexp.h>
#include <st/target.h>

#include <cassert>
#include <iostream>

auto main(int argc, char* argv[]) -> int {
  assert(argc == 2);

  std::string uart_path = argv[1];
  LOGN("Serial port: " << uart_path);

  // TODO: use argv[1] to select serial port type
  // TODO: pass serial params via cli too
  LinuxSerialPort serial_port(uart_path.c_str(), 115200, false);
  LispTarget target(serial_port);

  Sexp exp;
  std::cin >> exp;

  LOGN("Eval: " << exp);

  // Send to lisp super-computer
  std::string result = target.evaluate(exp.get());

  LOGN("Result: {" << result << "}");
  std::cout << result;

  return 0;
}
