// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include <algorithm>
#include <vector>

namespace krado::utils {

/// Convert supplied string to upper case.
///
/// @param text The string to convert upper case.
std::string to_upper(const std::string & text);

/// Convert supplied string to lower case.
///
/// @param text The string to convert upper case.
std::string to_lower(const std::string & text);

/// Check that `value` is equal to one of the `options`
///
/// @return `true` if `value` is one of the `options`, `false` otherwise
/// @param value Value to test
/// @param options Possible options
template <typename T>
inline bool
in(T value, const std::vector<T> & options)
{
    return std::any_of(options.cbegin(), options.cend(), [value](const T & o) {
        return value == o;
    });
}

template <>
inline bool
in<std::string>(std::string value, const std::vector<std::string> & options)
{
    std::string v = utils::to_lower(value);
    return std::any_of(options.cbegin(), options.cend(), [v](const std::string & o) {
        return v == utils::to_lower(o);
    });
}

template <>
inline bool
in<const char *>(const char * value, const std::vector<const char *> & options)
{
    std::string v = utils::to_lower(value);
    return std::any_of(options.cbegin(), options.cend(), [v](const std::string & o) {
        return v == utils::to_lower(o);
    });
}

} // namespace krado::utils
