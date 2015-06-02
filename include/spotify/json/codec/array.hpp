/*
 * Copyright (c) 2015 Spotify AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#pragma once

#include <deque>
#include <list>
#include <set>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <spotify/json/codec/standard.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename T, typename InnerCodec>
class array_t final {
 public:
  using object_type = T;

  static_assert(
      std::is_same<
          typename T::value_type,
          typename InnerCodec::object_type>::value,
      "Array container type must match inner codec type");

  array_t(InnerCodec inner_codec)
      : _inner_codec(inner_codec) {}

  void encode(const object_type &array, writer &w) const {
    w.add_array([&](writer &w) {
      for (const auto &element : array) {
        _inner_codec.encode(element, w);
      }
    });
  }

  object_type decode(decoding_context &context) const {
    object_type output;
    detail::advance_past_comma_separated(context, '[', ']', [&]{
      output.push_back(_inner_codec.decode(context));
    });
    return output;
  }

 private:
  InnerCodec _inner_codec;
};

template<typename T, typename InnerCodec>
array_t<T, InnerCodec> array(InnerCodec &&inner_codec) {
  return array_t<T, InnerCodec>(std::forward<InnerCodec>(inner_codec));
}

template<typename T>
struct standard_t<std::vector<T>> {
  static decltype(array<std::vector<T>>(standard<T>())) codec() {
    return array<std::vector<T>>(standard<T>());
  }
};

template<typename T>
struct standard_t<std::deque<T>> {
  static decltype(array<std::deque<T>>(standard<T>())) codec() {
    return array<std::deque<T>>(standard<T>());
  }
};

template<typename T>
struct standard_t<std::list<T>> {
  static decltype(array<std::list<T>>(standard<T>())) codec() {
    return array<std::list<T>>(standard<T>());
  }
};

template<typename T>
struct standard_t<std::set<T>> {
  static decltype(array<std::set<T>>(standard<T>())) codec() {
    return array<std::set<T>>(standard<T>());
  }
};

template<typename T>
struct standard_t<std::unordered_set<T>> {
  static decltype(array<std::unordered_set<T>>(standard<T>())) codec() {
    return array<std::unordered_set<T>>(standard<T>());
  }
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
