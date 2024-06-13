#pragma once

#include <iostream>

#include "reader.hpp"
#include "value.hpp"

namespace youdis {

class Parser {
 public:
  Parser(Reader &reader) : reader_(reader) {}

  Value *read() {
    auto type = static_cast<Type>(reader_.read_byte());
    if (type == Type::array) {
      return read_array();
    } else if (type == Type::bulk) {
      return read_bulk();
    }
    throw std::runtime_error("failed to read value.");
  }

 private:
  std::string readline() {
    std::string res;
    char pre = reader_.read_byte();
    while (true) {
      try {
        char b = reader_.read_byte();
        if (pre == '\r' && b == '\n') {
          break;
        }
        res.push_back(pre);
        pre = b;
      } catch (const std::exception &e) {
        throw std::runtime_error("failed to read line.");
      }
    }
    return res;
  }

  int read_int() {
    try {
      return stoi(readline());
    } catch (const std::exception &e) {
      std::cerr << "[Parser::read_int] " << e.what() << std::endl;
      throw std::runtime_error("failed to read int.");
    }
  }

  Value *read_array() {
    auto arr = new ArrayValue;
    try {
      int len = read_int();
      arr->arr.reserve(len);
      for (int i = 0; i < len; ++i) {
        arr->arr.push_back(read());
      }
      return arr;
    } catch (const std::exception &e) {
      std::cerr << "[Parser::read_array] " << e.what() << std::endl;
      throw std::runtime_error("failed to read array.");
    }
  }

  Value *read_bulk() {
    auto bulk = new BulkValue;
    try {
      int len = read_int();
      bulk->bulk.resize(len);
      reader_.read_n(bulk->bulk.data(), len);
      reader_.consumeline();
      return bulk;
    } catch (const std::exception &e) {
      std::cerr << "[Parser::read_bulk] " << e.what() << std::endl;
      throw std::runtime_error("failed to read bulk.");
    }
  }

  Reader &reader_;
};

}  // namespace youdis
