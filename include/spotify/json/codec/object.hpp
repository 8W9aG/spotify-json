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

#include <type_traits>
#include <unordered_map>

#include <spotify/json/codec/string.hpp>
#include <spotify/json/decoding_context.hpp>
#include <spotify/json/default_codec.hpp>
#include <spotify/json/detail/writer.hpp>
#include <spotify/json/key.hpp>

namespace spotify {
namespace json {
namespace codec {

template<typename T>
class object final {
 public:
  using object_type = T;

  template<
      typename U = T,
      typename = typename std::enable_if<std::is_default_constructible<U>::value>::type>
  object() {}

  object(const object<T> &object) = default;
  object(object<T> &&object) = default;

  template<
      typename Create,
      typename = typename std::enable_if<!std::is_same<
          typename std::decay<Create>::type,
          object>::value>::type>
  explicit object(Create &&create)
      : _construct(std::forward<Create>(create)) {}

  template<typename... Args>
  void optional(const std::string &name, Args &&...args) {
    add_field(name, false, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void required(const std::string &name, Args &&...args) {
    add_field(name, true, std::forward<Args>(args)...);
  }

  void encode(const object_type &value, detail::writer &w) const {
    w.add_object([&](detail::writer &w) {
      for (const auto &field : _field_list) {
        if (field.second->should_encode) {
          w.add_key(field.first);
          field.second->encode(value, w);
        }
      }
    });
  }

  object_type decode(decoding_context &context) const {
    std::vector<bool> encountered_required_fields(_fields.size());
    object_type output = construct(std::is_default_constructible<T>());
    const auto string_c = string();
    detail::advance_past_object(
        context,
        [string_c](decoding_context &context) {
          return string_c.decode(context);
        },
        [&](std::string &&key) {
          const auto field_it = _fields.find(key);
          if (field_it == _fields.end()) {
            // Ignore unknown key
            detail::advance_past_value(context);
          } else {
            const auto &field = *field_it->second;
            field.decode(output, context);
            if (field.required) {
              encountered_required_fields[field_it->second->field_id] = true;
            }
          }
        });


    const auto num_encountered_required_fields = std::count(
        encountered_required_fields.begin(),
        encountered_required_fields.end(),
        true);
    const auto is_missing_required_fields = (num_encountered_required_fields != _num_required_fields);
    detail::fail_if(context, is_missing_required_fields, "Missing required field(s)");
    return output;
  }

 private:
  T construct(std::true_type is_default_constructible) const {
    // Avoid the cost of an std::function invocation if no construct function
    // is provided.
    return _construct ? _construct() : object_type();
  }

  T construct(std::false_type is_default_constructible) const {
    // T is not default constructible. Because _construct must be set if T is
    // default constructible, there is no reason to test it in this case.
    return _construct();
  }

  struct field {
    field(bool should_encode, bool required, size_t field_id) :
        should_encode(should_encode),
        required(required),
        field_id(field_id) {}
    virtual ~field() = default;

    virtual void encode(const object_type &object, detail::writer &writer) const = 0;
    virtual void decode(object_type &object, decoding_context &context) const = 0;

    const bool should_encode;
    const bool required;
    const size_t field_id;
  };

  template<typename Codec>
  struct dummy_field final : public field {
    dummy_field(bool required, size_t field_id, Codec codec)
        : field(false, required, field_id),
          codec(std::move(codec)) {}

    void encode(const object_type &object, detail::writer &writer) const override {
    }

    void decode(object_type &object, decoding_context &context) const override {
      codec.decode(context);
    }

    Codec codec;
  };

  template<typename Member, typename Codec>
  struct member_field final : public field {
    member_field(bool required, size_t field_id, Codec codec, Member T::*member_pointer)
        : field(true, required, field_id),
          codec(std::move(codec)),
          member_pointer(member_pointer) {}

    void encode(const object_type &object, detail::writer &writer) const override {
      codec.encode(object.*member_pointer, writer);
    }

    void decode(object_type &object, decoding_context &context) const override {
      object.*member_pointer = codec.decode(context);
    }

    Codec codec;
    Member T::*member_pointer;
  };

  template<typename Member>
  void add_field(const std::string &name, bool required, Member T::*member) {
    add_field(name, required, member, default_codec<Member>());
  }

  template<typename Member, typename Codec>
  void add_field(const std::string &name, bool required, Member T::*member, Codec &&codec) {
    save_field(
        name,
        required, std::make_shared<member_field<Member, typename std::decay<Codec>::type>>(
            required, _fields.size(), std::forward<Codec>(codec), member));
  }

  template<typename Codec>
  void add_field(const std::string &name, bool required, Codec &&codec) {
    save_field(
        name,
        required, std::make_shared<dummy_field<typename std::decay<Codec>::type>>(
            required, _fields.size(), std::forward<Codec>(codec)));
  }

  void save_field(const std::string &name, bool required, const std::shared_ptr<field> &f) {
    const auto was_saved = _fields.insert(typename field_map::value_type(name, f)).second;
    if (was_saved) {
      _field_list.push_back(std::make_pair(key(name), f));
      if (required) {
        _num_required_fields++;
      }
    }
  }

  using field_list = std::vector<std::pair<key, std::shared_ptr<const field>>>;
  using field_map = std::unordered_map<std::string, std::shared_ptr<const field>>;
  /**
   * _construct may be unset, but only if T is default constructible. This is
   * enforced compile time by enabling the constructor that doesn't set it only
   * if T is default constructible.
   */
  const std::function<T ()> _construct;
  field_list _field_list;
  field_map _fields;
  size_t _num_required_fields = 0;
};

}  // namespace codec
}  // namespace json
}  // namespace spotify
