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

#include <spotify/json/decoding_context.hpp>
#include <spotify/json/detail/decoding_helpers.hpp>
#include <spotify/json/detail/primitive_encoder.hpp>
#include <spotify/json/standard.hpp>

namespace spotify {
namespace json {
namespace codec {

class boolean_t final : public detail::primitive_encoder<bool> {
 public:
  object_type decode(decoding_context &context) const {
    const char first = detail::peek(context);
    if (first == 'f') {
      detail::advance_past_false(context);
      return false;
    } else if (first == 't') {
      detail::advance_past_true(context);
      return true;
    } else {
      detail::fail(context, "Unexpected input, expected boolean");
      return false;
    }
  }
};

inline boolean_t boolean() {
  return boolean_t();
}

}  // namespace codec

template<>
struct standard_t<bool> {
  static codec::boolean_t codec() {
    return codec::boolean_t();
  }
};

}  // namespace json
}  // namespace spotify
