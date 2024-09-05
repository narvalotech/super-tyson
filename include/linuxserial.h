#ifndef LINUXSERIAL_H_
#define LINUXSERIAL_H_

#include <serial.h>
#include <unistd.h>

class LinuxSerialPort : public SerialPort {
private:
  int fd;

public:
  LinuxSerialPort(const char* device_path, unsigned int baud_rate, bool hw_flow_control) : fd(-1) {
    _init(device_path, baud_rate, hw_flow_control);
  }

  ~LinuxSerialPort() { close(fd); }

  auto send(const std::string_view view) -> int;
  auto getc(char& c) -> int;
  auto recv(std::string& dest) -> int;

private:
  auto _init(const char* device_path, unsigned int baud_rate, bool hw_flow_control) -> void;
  auto send_c_str(const char* data, size_t len) -> void;
  auto wait_for_data() -> bool;
  auto receive_c_str(char* buffer, size_t max_size) -> int;
};

#endif  // LINUXSERIAL_H_
