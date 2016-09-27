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

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <spotify/json/detail/char_traits.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)
BOOST_AUTO_TEST_SUITE(detail)

namespace {

const std::vector<char> digits =
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
const std::vector<char> hex_letters =
    { 'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F' };
const std::vector<char> non_hex_letters =
    { 'g', 'G', 'z', 'Z' };
const std::vector<char> whitespace = { ' ', '\t', '\n', '\r' };
const std::vector<char> zero = { '\0' };

template <typename Fn, typename Range>
void check(const Fn &fn, bool should_be_true, const Range &range) {
  for (const auto chr : range) {
    BOOST_CHECK(!should_be_true ^ fn(chr));
  }
}

}  // namespace

BOOST_AUTO_TEST_CASE(json_char_traits_is_space_should_detect_the_right_characters) {
  check(char_traits::is_space, true, whitespace);
  check(char_traits::is_space, false, digits);
  check(char_traits::is_space, false, hex_letters);
  check(char_traits::is_space, false, zero);
}

BOOST_AUTO_TEST_CASE(json_char_traits_is_digit_should_detect_the_right_characters) {
  check(char_traits::is_digit, true, digits);
  check(char_traits::is_digit, false, hex_letters);
  check(char_traits::is_digit, false, whitespace);
  check(char_traits::is_digit, false, zero);
}

BOOST_AUTO_TEST_CASE(json_char_traits_is_hex_digit_should_detect_the_right_characters) {
  check(char_traits::is_hex_digit, true, digits);
  check(char_traits::is_hex_digit, true, hex_letters);
  check(char_traits::is_hex_digit, false, non_hex_letters);
  check(char_traits::is_hex_digit, false, whitespace);
  check(char_traits::is_hex_digit, false, zero);
}

BOOST_AUTO_TEST_SUITE_END()  // detail
BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
