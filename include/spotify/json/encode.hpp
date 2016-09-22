/*
 * Copyright (c) 2015-2016 Spotify AB
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

#include <string>

#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/macros.hpp>
#include <spotify/json/encoding_context.hpp>

namespace spotify {
namespace json {

template <typename Codec>
json_never_inline std::string encode(const Codec &codec, const typename Codec::object_type &object) {
  encoding_context context;
  codec.encode(context, object);
  return std::string(static_cast<const char *>(context.data()), context.size());
}

template <typename Value>
json_never_inline std::string encode(const Value &value) {
  return encode(default_codec<Value>(), value);
}

}  // namespace json
}  // namespace spotify
