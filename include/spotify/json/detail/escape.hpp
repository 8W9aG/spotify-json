/*
 * Copyright (c) 2014-2016 Spotify AB
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

#include <cstdint>
#include <string>

#include <spotify/json/detail/macros.hpp>

namespace spotify {
namespace json {
namespace detail {

json_force_inline void write_escaped_c(uint8_t *&out, const uint8_t c) {
  static const char *HEX = "0123456789ABCDEF";
  static const char POPULAR_CONTROL_CHARACTERS[] = {
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u',
    'b', 't', 'n', 'u', 'f', 'r', 'u', 'u',
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u',
    'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u'
  };

  if (json_unlikely(c == '\\' || c == '"' || c == '/')) {
    *(out++) = '\\';
    *(out++) = c;
    return;
  }

  if (json_likely(c >= 0x20)) {
    *(out++) = c;
    return;
  }

  const auto control_character = POPULAR_CONTROL_CHARACTERS[c];
  *(out++) = '\\';
  *(out++) = control_character;

  if (json_unlikely(control_character == 'u')) {
    *(out++) = '0';
    *(out++) = '0';
    *(out++) = HEX[(c >> 4)];
    *(out++) = HEX[(c & 0x0F)];
  }
}

json_force_inline void write_escaped_1(uint8_t *&out, const uint8_t *&begin) {
  struct blob_1_t { uint8_t a; };
  const auto b = *reinterpret_cast<const blob_1_t *>(begin);
  write_escaped_c(out, b.a);
  begin += sizeof(blob_1_t);
}

json_force_inline void write_escaped_2(uint8_t *&out, const uint8_t *&begin) {
  struct blob_2_t { uint8_t a, b; };
  const auto b = *reinterpret_cast<const blob_2_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  begin += sizeof(blob_2_t);
}

json_force_inline void write_escaped_4(uint8_t *&out, const uint8_t *&begin) {
  struct blob_4_t { uint8_t a, b, c, d; };
  const auto b = *reinterpret_cast<const blob_4_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  write_escaped_c(out, b.c);
  write_escaped_c(out, b.d);
  begin += sizeof(blob_4_t);
}

json_force_inline void write_escaped_8(uint8_t *&out, const uint8_t *&begin) {
  struct blob_8_t { uint8_t a, b, c, d, e, f, g, h; };
  const auto b = *reinterpret_cast<const blob_8_t *>(begin);
  write_escaped_c(out, b.a);
  write_escaped_c(out, b.b);
  write_escaped_c(out, b.c);
  write_escaped_c(out, b.d);
  write_escaped_c(out, b.e);
  write_escaped_c(out, b.f);
  write_escaped_c(out, b.g);
  write_escaped_c(out, b.h);
  begin += sizeof(blob_8_t);
}

/**
 * \brief Escape a string for use in a JSON string as per RFC 4627.
 *
 * This escapes control characters (0x00 through 0x1F), as well as
 * backslashes and quotation marks.
 *
 * See: http://www.ietf.org/rfc/rfc4627.txt (Section 2.5)
 */
json_never_inline inline uint8_t *write_escaped(uint8_t *out, const uint8_t *begin, const uint8_t *end) {
  if (json_unaligned_2(begin) && (end - begin) >= 1) { write_escaped_1(out, begin); }
  if (json_unaligned_4(begin) && (end - begin) >= 2) { write_escaped_2(out, begin); }
  if (json_unaligned_8(begin) && (end - begin) >= 4) { write_escaped_4(out, begin); }
  while ((end - begin) >= 8) { write_escaped_8(out, begin); }
  if ((end - begin) >= 4) { write_escaped_4(out, begin); }
  if ((end - begin) >= 2) { write_escaped_2(out, begin); }
  if ((end - begin) >= 1) { write_escaped_1(out, begin); }
  return out;
}

}  // namespace detail
}  // namespace json
}  // namespace spotify
