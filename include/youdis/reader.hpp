#pragma once

#include <cstddef>

class Reader {
 public:
  virtual char read_byte() = 0;
  virtual void read_n(char *buf, size_t n) = 0;
  virtual void consumeline() = 0;
};
