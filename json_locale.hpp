// Copyright (c) 2014 Felix Bruns.

#pragma once

#include <locale.h>

// Include 'xlocale.h' header on Mac for newlocale,
// uselocale and freelocale functions.
#ifdef __APPLE__
#include <xlocale.h>
#endif  // __APPLE__

#ifdef _WIN32
// Define locale_t and category masks, for
// cross-platform compatibility on Windows.
typedef _locale_t locale_t;

#define LC_GLOBAL_LOCALE ((locale_t)-1)
#define LC_ALL_MASK      LC_ALL
#define LC_COLLATE_MASK  LC_COLLATE
#define LC_CTYPE_MASK    LC_CTYPE
#define LC_MONETARY_MASK LC_MONETARY
#define LC_NUMERIC_MASK  LC_NUMERIC
#define LC_TIME_MASK     LC_TIME
#endif  // _WIN32

namespace json {
namespace detail {

/**
 * \brief Create a new locale object.
 */
inline locale_t new_locale(int category_mask, const char *locale) {
#ifdef _WIN32
  return _create_locale(category_mask, locale);
#else
  return newlocale(category_mask, locale, NULL);
#endif
}

/**
 * \brief Use the given locale in the current thread.
 */
inline locale_t use_locale(locale_t new_locale) {
#ifdef _WIN32
  const bool is_per_thread = (_configthreadlocale(0) == _ENABLE_PER_THREAD_LOCALE);
  locale_t old_locale = is_per_thread ? _get_current_locale() : LC_GLOBAL_LOCALE;

  if (new_locale == LC_GLOBAL_LOCALE) {
    _configthreadlocale(_DISABLE_PER_THREAD_LOCALE);
  } else if(new_locale != NULL) {
    _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);

    for(int i = LC_MIN; i <= LC_MAX; i++) {
      const pthreadlocinfo locinfo = new_locale->locinfo;

      setlocale(i, locinfo->lc_category[i].locale);
      _wsetlocale(i, locinfo->lc_category[i].wlocale);
    }
  }

  return old_locale;
#else
  return uselocale(new_locale);
#endif
}

/**
 * \brief Free resources allocated for a locale object.
 */
inline void free_locale(locale_t loc) {
#ifdef _WIN32
  _free_locale(loc);
#else
  freelocale(loc);
#endif
}

}  // namespace detail

class scoped_locale {
 public:
  scoped_locale(int category_mask, const char *locale)
    : _new_locale(detail::new_locale(category_mask, locale)),
      _old_locale(detail::use_locale(_new_locale)) {}

  virtual ~scoped_locale() {
    detail::use_locale(_old_locale);
    detail::free_locale(_new_locale);
  }

 private:
  locale_t _new_locale;
  locale_t _old_locale;
};

}  // namespace json
