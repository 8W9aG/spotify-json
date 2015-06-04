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
#include <spotify/json/detail/writer.hpp>

namespace spotify {
namespace json {
namespace codec {

/**
 * The interface that codecs must conform to is as follows:
 *
 * Please note that this class only exists for documentation purposes: It should
 * not actually be used in code anywhere.
 */
class codec_interface final {
 public:
  /**
   * This class exists only for documentation purposes.
   */
  codec_interface() = delete;

  /**
   * Typedef for the type that this codec pases to and from.
   */
  using object_type = void *;

  /**
   * Write an object to a writer.
   */
  void encode(const object_type &value, detail::writer &writer) const;

  /**
   * Parse the given JSON. The string to parse begins at context.position and
   * ends at context.end. It is not an error if end is beyond the object that
   * this codec parses.
   *
   * If parsing succeeds, position should be set to point to the character after
   * the last character that was parsed. If parsing fails, context.error should
   * be set to a non-empty string and context.position should point to where the
   * error occured.
   *
   * If parsing fails, object_type should be ignored by the caller.
   *
   * decode will never be called with a contest that has_failed().
   */
  object_type decode(decoding_context &context) const;
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
