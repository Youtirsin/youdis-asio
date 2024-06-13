#pragma once

#include <string>
#include <vector>

namespace youdis {

enum class Type : char {
  string = '+',
  error = '-',
  integer = ':',
  bulk = '$',
  array = '*'
};

struct Value {
  Value(Type t) : type(t) {}
  virtual ~Value() {}
  virtual std::string marshal() = 0;

  Type type;
};

struct StringValue : public Value {
  StringValue(const std::string& s = "") : str(s), Value(Type::string) {}

  std::string marshal() {
    return std::string(1, static_cast<char>(Type::string)) + str + "\r\n";
  }

  std::string str;
};

struct IntegerValue : public Value {
  IntegerValue(int n) : num(n), Value(Type::integer) {}

  std::string marshal() {
    return std::string(1, static_cast<char>(Type::integer)) +
           std::to_string(num) + "\r\n";
  }

  int num;
};

struct BulkValue : public Value {
  BulkValue(const std::string& b = "") : Value(Type::bulk), bulk(b) {}

  std::string marshal() {
    return std::string(1, static_cast<char>(Type::bulk)) +
           std::to_string(bulk.size()) + "\r\n" + bulk + "\r\n";
  }

  std::string bulk;
};

struct ArrayValue : public Value {
  ArrayValue() : Value(Type::array) {}

  ~ArrayValue() {
    for (auto v : arr) {
      delete v;
    }
  }

  std::string marshal() {
    std::string bytes = std::string(1, static_cast<char>(Type::array)) +
                        std::to_string(arr.size()) + "\r\n";
    for (auto v : arr) {
      bytes += v->marshal();
    }
    return bytes;
  }

  std::vector<Value*> arr;
};

struct ErrorValue : public Value {
  ErrorValue(const std::string& e = "") : Value(Type::error), error(e) {}

  std::string marshal() {
    return std::string(1, static_cast<char>(Type::error)) + error + "\r\n";
  }

  std::string error;
};

struct NullValue : public Value {
  NullValue() : Value(Type::bulk) {}

  std::string marshal() {
    return std::string(1, static_cast<char>(Type::bulk)) + "-1\r\n";
  }
};

template <class T>
T* getInType(Value* value) {
  if ((std::is_same<T, StringValue>::value && value->type == Type::string) ||
      (std::is_same<T, IntegerValue>::value && value->type == Type::integer) ||
      (std::is_same<T, BulkValue>::value && value->type == Type::bulk) ||
      (std::is_same<T, ArrayValue>::value && value->type == Type::array) ||
      (std::is_same<T, ErrorValue>::value && value->type == Type::error)) {
    return static_cast<T*>(value);
  }
  return nullptr;
}

}  // namespace youdis
