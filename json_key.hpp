// Copyright 2014 Felix Bruns and Johan Lindström.

#pragma once
#ifndef JSON_KEY_HPP_
#define JSON_KEY_HPP_

#include <string>

#include "json_escape.hpp"

namespace json {

class key {
 public:
  explicit key(const char *raw)
      : _storage("\""),
        _ref(write_escaped(_storage, raw, null_terminated_end_iterator())),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

  template<typename Iterable>
  explicit key(const Iterable &iterable)
      : _storage("\""),
        _ref(write_escaped(_storage, iterable.begin(), iterable.end())),
        data(_ref.append("\"").data()),
        size(_ref.size()) {}

 private:
  std::string _storage;
  std::string &_ref;

 public:
  const char *data;
  const size_t size;
};

}  // namespace json

#endif  // JSON_KEY_HPP_
