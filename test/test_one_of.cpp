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
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/one_of.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/string.hpp>
#include <spotify/json/encode_decode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template<typename Codec>
typename Codec::object_type test_decode(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK_EQUAL(c.error, "");
  BOOST_CHECK_EQUAL(c.position, c.end);
  return obj;
}

template<typename Codec>
void test_decode_fail(const Codec &codec, const std::string &json) {
  decoding_context c(json.c_str(), json.c_str() + json.size());
  auto obj = codec.decode(c);
  BOOST_CHECK(c.has_failed());
}

struct example_t {
  std::string value;
};

}  // namespace

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_construct) {
  one_of_t<object<example_t>> codec;
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_construct_with_helper) {
  one_of(object<example_t>(), object<example_t>());
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_encode_with_first) {
  object<example_t> first;
  first.required("a", &example_t::value);

  object<example_t> second;
  second.required("b", &example_t::value);

  const auto codec = one_of(first, second);

  example_t value;
  value.value = "val";

  BOOST_CHECK_EQUAL(encode(codec, value), "{\"a\":\"val\"}");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_decode_with_first_if_possible) {
  object<example_t> first;
  first.required("a", &example_t::value);

  object<example_t> second;

  const auto codec = one_of(first, second);
  const auto example = test_decode(codec, "{\"a\":\"first\"}");
  BOOST_CHECK_EQUAL(example.value, "first");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_decode_with_second_if_needed) {
  object<example_t> first;
  first.required("a", &example_t::value);

  object<example_t> second;
  second.required("b", &example_t::value);

  const auto codec = one_of(first, second);
  const auto example = test_decode(codec, "{\"b\":\"second\"}");
  BOOST_CHECK_EQUAL(example.value, "second");
}

BOOST_AUTO_TEST_CASE(json_codec_one_of_should_fail_decode_if_all_fail) {
  object<example_t> first;
  first.required("a", &example_t::value);

  object<example_t> second;
  second.required("b", &example_t::value);

  const auto codec = one_of(first, second);
  test_decode_fail(codec, "{}");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
