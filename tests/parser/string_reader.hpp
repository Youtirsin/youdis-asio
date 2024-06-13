#pragma once

#include <stdexcept>
#include <string>

#include "../../include/youdis/reader.hpp"

class StringReader : public Reader {
 public:
  StringReader(const std::string &str) : str_(str), idx_(0) {}

  char read_byte() {
    if (idx_ == str_.size()) {
      throw std::runtime_error("failed to read string byte. out of boundary.");
    }
    return str_[idx_++];
  }

  void read_n(char *buf, size_t n) {
    for (size_t i = 0; i < n; ++i) {
      *buf = str_[idx_];
      ++idx_;
      ++buf;
      if (idx_ == str_.size()) {
        return;
      }
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
        break;
      }
    }
  }

 private:
  size_t idx_;
  const std::string &str_;
};
