#include "../../include/youdis/parser.hpp"

#include <iostream>

#include "string_reader.hpp"
#include "../../include/youdis/value.hpp"

int main(int argc, char *argv[]) {
  using namespace youdis;

  std::string str = "*2\r\n$5\r\nhello\r\n$5\r\nworld\r\n";
  StringReader strReader(str);
  Parser parser(strReader);

  auto val = parser.read();
  std::cout << static_cast<char>(val->type) << std::endl;

  auto arr = static_cast<ArrayValue *>(val);
  std::cout << arr->arr.size() << std::endl;

  for (auto v : arr->arr) {
    std::cout << static_cast<char>(v->type) << std::endl;
    auto bv = static_cast<BulkValue *>(v);
    std::cout << bv->bulk << std::endl;
  }

  delete val;

  return 0;
}
