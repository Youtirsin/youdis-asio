#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "data.hpp"
#include "value.hpp"

namespace youdis {

inline Value *ping(std::vector<Value *> &args) {
  if (args.empty()) {
    return new StringValue("PONG");
  }
  return new StringValue(getInType<BulkValue>(args.front())->bulk);
}

inline Value *set(std::vector<Value *> &args) {
  if (args.size() != 2) {
    return new ErrorValue("wrong number of arguments for 'set' command.");
  }
  auto key = getInType<BulkValue>(args.front());
  auto value = getInType<BulkValue>(args.back());
  if (!key || !value) {
    return new ErrorValue("invalid arguments for 'set' command.");
  }
  Set::set(key->bulk, value->bulk);
  return new StringValue("OK");
}

inline Value *get(std::vector<Value *> &args) {
  if (args.size() != 1) {
    return new ErrorValue("wrong number of arguments for 'get' command.");
  }
  auto key = getInType<BulkValue>(args.front());
  if (!key) {
    return new ErrorValue("invalid arguments for 'get' command.");
  }
  auto value = Set::get(key->bulk);
  if (!value) {
    return new NullValue();
  }
  return new BulkValue(value.value());
}

inline Value *hset(std::vector<Value *> &args) {
  if (args.size() != 3) {
    return new ErrorValue("wrong number of arguments for 'hset' command.");
  }
  auto outer_key = getInType<BulkValue>(args[0]);
  auto inner_key = getInType<BulkValue>(args[1]);
  auto value = getInType<BulkValue>(args[2]);
  if (!outer_key || !inner_key || !value) {
    return new ErrorValue("invalid arguments for 'hset' command.");
  }
  HSet::set(outer_key->bulk, inner_key->bulk, value->bulk);
  return new StringValue("OK");
}

inline Value *hget(std::vector<Value *> &args) {
  if (args.size() != 2) {
    return new ErrorValue("wrong number of arguments for 'hget' command.");
  }
  auto outer_key = getInType<BulkValue>(args.front());
  auto inner_key = getInType<BulkValue>(args.back());
  if (!outer_key || !inner_key) {
    return new ErrorValue("invalid arguments for 'hget' command.");
  }
  auto value = HSet::get(outer_key->bulk, inner_key->bulk);
  if (!value) {
    return new NullValue();
  }
  return new BulkValue(value.value());
}

inline std::function<Value *(std::vector<Value *> &)> handlers(
    const std::string &command) {
  static std::unordered_map<std::string,
                            std::function<Value *(std::vector<Value *> &)>>
      funcs = {{"PING", ping},
               {"SET", set},
               {"GET", get},
               {"HSET", hset},
               {"HGET", hget}};

  auto cmd = command;
  for (char &c : cmd) {
    c = toupper(c);
  }
  if (funcs.find(cmd) == funcs.end()) {
    throw std::runtime_error("failed to find handler for command.");
  }
  return funcs[cmd];
}

}  // namespace youdis
