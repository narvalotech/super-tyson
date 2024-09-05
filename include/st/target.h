#ifndef TARGET_H_
#define TARGET_H_

#include <st/serial.h>

#include <string>

class LispTarget {
public:
  LispTarget(SerialPort& port) : _port(port) { _init(port); };
  ~LispTarget(){};

  auto evaluate(const std::string& input) -> std::string;

private:
  auto _init(SerialPort& port) -> void;

protected:
  SerialPort& _port;
};

#endif  // TARGET_H_
