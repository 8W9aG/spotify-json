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

#pragma once

#include <boost/optional.hpp>

#include "../json_pair.hpp"
#include "../json_writer.hpp"

namespace spotify {
namespace json {

template<typename stream_type, typename options_type, typename T>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const boost::optional<T> &optional) {
  if (optional) {
    writer << optional.get();
  }
  return writer;
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const pair<K, boost::optional<V> > &pair) {
  if (pair.value) {
    writer.add_pair(pair.key, pair.value.get());
  }
  return writer;
}

template<typename stream_type, typename options_type, typename K, typename V>
basic_writer<stream_type, options_type> &operator <<(basic_writer<stream_type, options_type> &writer, const std::pair<K, boost::optional<V> > &pair) {
  if (pair.second) {
    writer.add_pair(pair.first, pair.second.get());
  }
  return writer;
}

}  // namespace json
}  // namespace spotify
