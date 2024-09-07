#include <errno.h>
#include <fcntl.h>
#include <st/linuxserial.h>
#include <st/serial.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#include <cstring>
#include <string>

#define LOGN(str) std::cerr << str << std::endl
#define LOG(str) std::cerr << str;

auto LinuxSerialPort::send(const std::string_view view) -> int {
  send_c_str(view.data(), view.length());

  return 0;
}

auto LinuxSerialPort::getc(char& c) -> int {
  if (!wait_for_data()) {
    // The wait timed-out, we won't read anything
    return 0;
  }

  ssize_t bytes_read = read(fd, &c, 1);
  if (bytes_read < 0) {
    throw std::runtime_error("Error reading from serial port");
  }
  // LOGN("got[" << c << "]");

  return bytes_read;
}

auto LinuxSerialPort::recv(std::string& dest) -> int {
  // TODO: implement
  return 0;
}

auto LinuxSerialPort::send_c_str(const char* data, size_t len) -> void {
  ssize_t bytes_written = write(fd, data, len);
  if (bytes_written < 0) {
    throw std::runtime_error("Error writing to serial port");
  }
}

auto LinuxSerialPort::wait_for_data() -> bool {
  fd_set read_fdset;
  FD_ZERO(&read_fdset);
  FD_SET(fd, &read_fdset);

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000;  // 100ms timeout

  // LOGN("wait for data");
  int sr = select(fd + 1, &read_fdset, NULL, NULL, &tv);
  if (sr < 0) {
    throw std::runtime_error("Error in select");
  }

  if (!FD_ISSET(fd, &read_fdset)) {
    return false;
  }

  return true;
}

auto LinuxSerialPort::receive_c_str(char* buffer, size_t max_size) -> int {
  wait_for_data();

  // LOGN("read");
  ssize_t bytes_read = read(fd, buffer, max_size - 1);
  if (bytes_read < 0) {
    throw std::runtime_error("Error reading from serial port");
  }
  buffer[bytes_read] = '\0';
  return static_cast<int>(bytes_read);
}

auto LinuxSerialPort::_init(const char* device_path, unsigned int baud_rate, bool hw_flow_control)
    -> void {
  fd = open(device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd < 0) {
    throw std::runtime_error("Failed to open serial port");
  }

  struct termios tty;
  memset(&tty, 0, sizeof tty);

  if (tcgetattr(fd, &tty) != 0) {
    throw std::runtime_error("Failed to get terminal attributes");
  }

  // FIXME: stop hardcoding the baudrate
  if (baud_rate == 115200) {
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);
  } else {
    throw std::runtime_error("Any color you want, as long as it's 115200bps");
  }

  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_cflag |= (hw_flow_control ? CRTSCTS : 0);

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(ICRNL | INLCR);
  tty.c_oflag &= ~OPOST;
  tty.c_oflag &= ~ONLCR;
  tty.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG);

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    throw std::runtime_error("Failed to set terminal attributes");
  }

  if (tcflush(fd, TCIFLUSH) != 0) {
    throw std::runtime_error("Failed to flush serial buffer");
  }
}
