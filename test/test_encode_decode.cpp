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

#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/object.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

namespace {

struct custom_obj {
  std::string val;
};

codec::object_t<custom_obj> custom_codec() {
  auto codec = codec::object<custom_obj>();
  codec.required("a", &custom_obj::val);
  return codec;
}

}

template <>
struct default_codec_t<custom_obj> {
  static codec::object_t<custom_obj> codec() {
    auto codec = codec::object<custom_obj>();
    codec.required("x", &custom_obj::val);
    return codec;
  }
};

BOOST_AUTO_TEST_CASE(json_encode_should_encode_into_string_with_custom_codec) {
  custom_obj obj;
  obj.val = "c";
  BOOST_CHECK_EQUAL(encode(custom_codec(), obj), R"({"a":"c"})");
}

BOOST_AUTO_TEST_CASE(json_encode_should_encode_into_string) {
  custom_obj obj;
  obj.val = "d";
  BOOST_CHECK_EQUAL(encode(obj), R"({"x":"d"})");
}

BOOST_AUTO_TEST_CASE(json_decode_should_decode_from_bytes_with_custom_codec) {
  static const char * const kData = R"({"a":"e"})";
  const auto obj = decode(custom_codec(), kData, strlen(kData));
  BOOST_CHECK_EQUAL(obj.val, "e");
}

BOOST_AUTO_TEST_CASE(json_decode_should_encode_from_string_with_custom_codec) {
  const auto obj = decode(custom_codec(), R"({"a":"g"})");
  BOOST_CHECK_EQUAL(obj.val, "g");
}

BOOST_AUTO_TEST_CASE(json_decode_should_encode_from_string) {
  const auto obj = decode<custom_obj>(R"({"x":"h"})");
  BOOST_CHECK_EQUAL(obj.val, "h");
}

BOOST_AUTO_TEST_CASE(json_decode_should_accept_trailing_space) {
  const auto obj = decode<custom_obj>(R"({"x":"h"}  )");
  BOOST_CHECK_EQUAL(obj.val, "h");
}

BOOST_AUTO_TEST_CASE(json_decode_should_accept_leading_space) {
  const auto obj = decode<custom_obj>(R"(  {"x":"h"})");
  BOOST_CHECK_EQUAL(obj.val, "h");
}

BOOST_AUTO_TEST_CASE(json_decode_should_throw_on_failure) {
  try {
    decode<custom_obj>("{}");  // Missing field
    BOOST_ASSERT(!"Should not reach this point");
  } catch (const decode_exception &) {
  }
}

BOOST_AUTO_TEST_CASE(json_decode_should_throw_on_unexpected_trailing_input) {
  try {
    decode<custom_obj>(R"({"x":"h"} invalid)");
    BOOST_ASSERT(!"Should not reach this point");
  } catch (const decode_exception &) {
  }
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_decode_from_bytes_with_custom_codec) {
  static const char * const kData = R"({"a":"e"})";
  custom_obj obj;
  BOOST_CHECK(try_decode(obj, custom_codec(), kData, strlen(kData)));
  BOOST_CHECK_EQUAL(obj.val, "e");
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_encode_from_string_with_custom_codec) {
  custom_obj obj;
  BOOST_CHECK(try_decode(obj, custom_codec(), R"({"a":"g"})"));
  BOOST_CHECK_EQUAL(obj.val, "g");
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_encode_from_string) {
  custom_obj obj;
  BOOST_CHECK(try_decode(obj, R"({"x":"h"})"));
  BOOST_CHECK_EQUAL(obj.val, "h");
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_report_failure) {
  custom_obj obj;
  BOOST_CHECK(!try_decode(obj, "{}"));  // Missing field
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_fail_on_unexpected_trailing_input) {
  custom_obj obj;
  BOOST_CHECK(!try_decode(obj, R"({"x":"h"} invalid)"));
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_accept_trailing_space) {
  custom_obj obj;
  BOOST_CHECK(try_decode(obj, R"({"x":"h"}  )"));
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_accept_leading_space) {
  custom_obj obj;
  BOOST_CHECK(try_decode(obj, R"(  {"x":"h"})"));
}

BOOST_AUTO_TEST_CASE(json_try_decode_should_accept_utf8) {
  custom_obj obj;
  BOOST_CHECK(try_decode(obj, u8"{\"x\":\"\u9E21\"}"));
  BOOST_CHECK_EQUAL(u8"\u9E21", obj.val);
}

BOOST_AUTO_TEST_CASE(json_try_decode_partial_should_succeed_even_with_trailing_input) {
  custom_obj obj;
  std::string input = R"({"x":"hey"}, "foobar")";
  json::decoding_context ctx(input.data(), input.size());
  BOOST_CHECK(try_decode_partial(obj, json::default_codec<custom_obj>(), ctx));
  BOOST_CHECK_EQUAL("hey", obj.val);
}

BOOST_AUTO_TEST_CASE(json_try_decode_partial_should_skip_preceding_whitespace) {
  custom_obj obj;
  std::string input = R"(          {"x":"hey"})";
  json::decoding_context ctx(input.data(), input.size());
  BOOST_CHECK(try_decode_partial(obj, json::default_codec<custom_obj>(), ctx));
  BOOST_CHECK_EQUAL("hey", obj.val);
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
