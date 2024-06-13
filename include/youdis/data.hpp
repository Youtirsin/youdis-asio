#pragma once

#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace youdis {

class Set {
 public:
  static Set &instance() {
    static Set instance;
    return instance;
  }

  static void set(const std::string &key, const std::string &value) {
    instance().set_impl(key, value);
  }

  static std::optional<std::string> get(const std::string &key) {
    return instance().get_impl(key);
  }

 private:
  Set() = default;
  Set(const Set &) = delete;
  Set &operator=(const Set &) = delete;

  void set_impl(const std::string &key, const std::string &value) {
    std::unique_lock<std::shared_mutex> guard(mtx_);
    data_[key] = value;
  }

  std::optional<std::string> get_impl(const std::string &key) const {
    std::shared_lock<std::shared_mutex> guard(mtx_);
    auto it = data_.find(key);
    if (it != data_.end()) {
      return it->second;
    } else {
      return std::nullopt;
    }
  }

  std::unordered_map<std::string, std::string> data_;
  mutable std::shared_mutex mtx_;
};

class HSet {
 public:
  static HSet &instance() {
    static HSet instance;
    return instance;
  }

  static void set(const std::string &outer_key, const std::string &inner_key,
                  const std::string &value) {
    instance().set_impl(outer_key, inner_key, value);
  }

  static std::optional<std::string> get(const std::string &outer_key,
                                        const std::string &inner_key) {
    return instance().get_impl(outer_key, inner_key);
  }

  static void erase(const std::string &outer_key,
                    const std::string &inner_key) {
    instance().erase_impl(outer_key, inner_key);
  }

 private:
  HSet() = default;
  HSet(const HSet &) = delete;
  HSet &operator=(const HSet &) = delete;

  void set_impl(const std::string &outer_key, const std::string &inner_key,
                const std::string &value) {
    std::unique_lock<std::shared_mutex> guard(mtx_);
    data_[outer_key][inner_key] = value;
  }

  std::optional<std::string> get_impl(const std::string &outer_key,
                                      const std::string &inner_key) const {
    std::shared_lock<std::shared_mutex> guard(mtx_);
    auto outer_it = data_.find(outer_key);
    if (outer_it != data_.end()) {
      auto inner_it = outer_it->second.find(inner_key);
      if (inner_it != outer_it->second.end()) {
        return inner_it->second;
      }
    }
    return std::nullopt;
  }

  void erase_impl(const std::string &outer_key, const std::string &inner_key) {
    std::unique_lock<std::shared_mutex> guard(mtx_);
    auto outer_it = data_.find(outer_key);
    if (outer_it != data_.end()) {
      outer_it->second.erase(inner_key);
      if (outer_it->second.empty()) {
        data_.erase(outer_key);
      }
    }
  }

  std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
      data_;
  mutable std::shared_mutex mtx_;
};

}  // namespace youdis
