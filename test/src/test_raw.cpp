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

#include <cstdint>
#include <cstdlib>
#include <string>

#include <boost/test/unit_test.hpp>

#include <spotify/json/codec/array.hpp>
#include <spotify/json/codec/object.hpp>
#include <spotify/json/codec/raw.hpp>
#include <spotify/json/decode.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/encode.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(codec)

namespace {

template <typename value_type>
struct foobar_t {
  value_type value;
};

template <typename value_type = raw_ref>
void verify_decode_raw(const std::string &raw_value) {
  const auto codec = raw<value_type>();
  const auto decoded = json::decode(codec, raw_value);
  BOOST_CHECK_EQUAL(raw_value, std::string(decoded.data(), decoded.data() + decoded.size()));
}

}  // namespace

/*
 * Constructing
 */

BOOST_AUTO_TEST_CASE(json_codec_raw_ref_should_construct_from_data_size) {
  std::string raw = "true";
  raw_ref ref(raw.data(), raw.size());

  BOOST_CHECK_EQUAL(ref.data(), raw.data());
  BOOST_CHECK_EQUAL(ref.size(), raw.size());
}

BOOST_AUTO_TEST_CASE(json_codec_raw_ref_should_construct_from_begin_end) {
  std::string raw = "true";
  raw_ref ref(raw.data(), raw.data() + raw.size());

  BOOST_CHECK_EQUAL(ref.data(), raw.data());
  BOOST_CHECK_EQUAL(ref.size(), raw.size());
}

BOOST_AUTO_TEST_CASE(json_codec_raw_ref_should_convert_to_decode_context) {
  std::string raw = "true";
  raw_ref ref(raw.data(), raw.size());
  decode_context context(ref);

  const auto begin = raw.data();
  const auto end = begin + raw.size();
  BOOST_CHECK_EQUAL(context.begin, begin);
  BOOST_CHECK_EQUAL(context.position, begin);
  BOOST_CHECK_EQUAL(context.end, end);
}

/*
 * Decoding
 */

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_array) {
  verify_decode_raw("[1, 2, 3]");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_object) {
  verify_decode_raw(R"({"hey":"yo"})");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_boolean) {
  verify_decode_raw("true");
  verify_decode_raw("false");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_null) {
  verify_decode_raw("null");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_string) {
  verify_decode_raw("\"foobar\"");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_number) {
  verify_decode_raw("123");
  verify_decode_raw("123.456");
  verify_decode_raw("-123.456");
  verify_decode_raw("-123.456e+45");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_deep_json) {
  // This is deep enough to blow the stack if the raw codec is implemented using
  // simple recursion. The failure case of this unit test is that it crashes.
  const auto depth = 1000000;

  std::string str;
  str.reserve(depth * 2);
  for (auto i = 0; i < depth; i++) { str += '['; }
  for (auto i = 0; i < depth; i++) { str += ']'; }

  verify_decode_raw(str);
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_into_string) {
  verify_decode_raw<std::string>("[1, 2, 3]");
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_decode_into_vector) {
  verify_decode_raw<std::vector<uint8_t>>("[1, 2, 3]");
}

/*
 * Encoding
 */

BOOST_AUTO_TEST_CASE(json_codec_raw_should_encode_ref_as_is) {
  std::string data = "some junk";
  raw_ref ref(data.data(), data.size());
  BOOST_CHECK_EQUAL(encode(ref), data);
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_encode_string_as_is) {
  std::string data = "some junk";
  BOOST_CHECK_EQUAL(encode(raw<std::string>(), data), data);
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_encode_vector_as_is) {
  std::string data = "some junk";
  std::vector<uint8_t> vec(data.data(), data.data() + data.size());
  BOOST_CHECK_EQUAL(encode(raw<std::vector<uint8_t>>(), vec), data);
}

BOOST_AUTO_TEST_CASE(json_codec_raw_should_encode_with_separators) {
  std::string raw = "{}";
  raw_ref ref(raw.data(), raw.size());
  std::vector<raw_ref> refs{ref, ref, ref};
  BOOST_CHECK_EQUAL(encode(refs), "[{},{},{}]");
}

BOOST_AUTO_TEST_SUITE_END()  // codec
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
