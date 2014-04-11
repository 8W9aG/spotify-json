/*
 * Copyright (c) 2014 Spotify AB
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

#define BOOST_TEST_MODULE json_test

#include <string>
#include <map>
#include <utility>

#include <boost/optional.hpp>
#include <boost/test/unit_test.hpp>

#include <spotify/json/extension/boost.hpp>
#include <spotify/json/extension/std.hpp>
#include <spotify/json/pair.hpp>
#include <spotify/json/writer.hpp>

BOOST_AUTO_TEST_SUITE(spotify)
BOOST_AUTO_TEST_SUITE(json)

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_vector_with_boost_optional) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::vector<boost::optional<int> > vector;
  vector.push_back(boost::optional<int>(1));
  vector.push_back(boost::optional<int>(2));
  vector.push_back(boost::optional<int>());  // empty
  writer << vector;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[1,2]", json);
}

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_map_with_boost_optional) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::map<std::string, boost::optional<int> > map;
  map["a"] = boost::optional<int>(1);
  map["b"] = boost::optional<int>(2);
  map["c"] = boost::optional<int>();  // empty
  writer << map;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("{\"a\":1,\"b\":2}", json);
}

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_vector) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::vector<std::string> vector;
  vector.push_back("a");
  vector.push_back("b");
  vector.push_back("c");
  writer << vector;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[\"a\",\"b\",\"c\"]", json);
}

BOOST_AUTO_TEST_CASE(json_overload_stream_operator_std_deque) {
  json::buffer buffer;
  json::writer writer(buffer);

  std::deque<std::string> deque;
  deque.push_back("a");
  deque.push_back("b");
  deque.push_back("c");
  writer << deque;

  std::string json(buffer.data(), buffer.size());

  BOOST_CHECK_EQUAL("[\"a\",\"b\",\"c\"]", json);
}

struct test_options {
  enum DefaultValue { DEFAULT_VALUE = 12 };

  test_options()
    : value(DEFAULT_VALUE) {}

  test_options(int value_)
    : value(value_) {}

  int value;
};

BOOST_AUTO_TEST_CASE(json_options_should_be_set_with_constructor) {
  test_options options(123);
  json::buffer buffer;
  json::writer_with_options<test_options> writer(buffer, options);

  BOOST_CHECK_EQUAL(writer.options().value, 123);
}

BOOST_AUTO_TEST_CASE(json_options_should_be_settable_with_stream_operator) {
  test_options options(123);
  json::buffer buffer;
  json::writer_with_options<test_options> writer(buffer, options);

  writer << test_options(456);

  BOOST_CHECK_EQUAL(writer.options().value, 456);
}

BOOST_AUTO_TEST_CASE(json_options_should_use_default_constructor) {
  json::buffer buffer;
  json::writer_with_options<test_options> writer(buffer);

  BOOST_CHECK_EQUAL(writer.options().value, test_options::DEFAULT_VALUE);
}

BOOST_AUTO_TEST_SUITE_END()  // json
BOOST_AUTO_TEST_SUITE_END()  // spotify
