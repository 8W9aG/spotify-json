/*
 * Copyright (c) 2014-2015 Spotify AB
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

#if _MSC_VER
  #define json_force_inline __forceinline
  #define json_never_inline __declspec(noinline)
  #define json_noreturn __declspec(noreturn)
  #define json_likely(expr) (expr)
  #define json_unlikely(expr) (expr)
#elif __GNUC__
  #define json_force_inline __attribute__((always_inline)) inline
  #define json_never_inline __attribute__((noinline))
  #define json_noreturn __attribute__((noreturn))
  #define json_likely(expr) __builtin_expect(!!(expr), 1)
  #define json_unlikely(expr) __builtin_expect(!!(expr), 0)
#else
  #define json_force_inline inline
  #define json_never_inline
  #define json_noreturn
  #define json_likely(expr) (expr)
  #define json_unlikely(expr) (expr)
#endif  // _MSC_VER

#ifdef max
  #undef max
#endif  // max

#ifdef min
  #undef min
#endif  // min
