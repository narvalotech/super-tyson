#ifndef SERIAL_H_
#define SERIAL_H_

#include <iostream>
#include <chrono>

class SerialPort {
public:
  SerialPort(){};
  SerialPort(const char* name, unsigned int baudrate, bool hwfc){};
  virtual ~SerialPort() {}

  virtual auto send(const std::string_view view) -> int = 0;
  virtual auto recv(std::string& dest) -> int = 0;
  virtual auto getc(char& c) -> int = 0;
  virtual auto wait_for_data() -> bool = 0;

  auto null_route(int time_ms) -> void {
    auto end = current_time() + (time_ms * 1000);
    while (current_time() < end) {
      char c;
      wait_for_data();
      getc(c);
    }
  }

private:
  auto current_time() -> int64_t {
    // Generated by Phind
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return micros.count();
  }
};

#endif // SERIAL_H_
