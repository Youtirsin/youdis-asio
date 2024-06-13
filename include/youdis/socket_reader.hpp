#pragma once

#include <boost/asio.hpp>

#include "reader.hpp"

using boost::asio::ip::tcp;

namespace youdis {

class SocketReader : public Reader {
 public:
  SocketReader(tcp::socket &socket) : socket_(socket) {}

  char read_byte() {
    char bytes[1] = {0};
    size_t read = 0;
    try {
      read = socket_.read_some(boost::asio::buffer(bytes, 1));
    } catch (const std::exception &e) {
      throw std::runtime_error("failed to read byte.");
    }
    if (read != 1) {
      throw std::runtime_error("failed to read byte.");
    }
    return bytes[0];
  }

  void read_n(char *buf, size_t n) {
    size_t i = 0;
    try {
      while (i < n) {
        i += socket_.read_some(boost::asio::buffer(buf + i, n));
      }
    } catch (const std::exception &e) {
      throw std::runtime_error("failed to read n bytes.");
    }
  }

  void consumeline() {
    char pre = read_byte();
    while (true) {
      try {
        char b = read_byte();
        if (pre == '\r' && b == '\n') {
          break;
        }
        pre = b;
      } catch (const std::exception &e) {
        throw std::runtime_error("failed to consume line.");
      }
    }
  }

 private:
  static constexpr size_t max_length = 1024;

  tcp::socket &socket_;
};

}  // namespace youdis
