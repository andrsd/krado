// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <vector>
#include <map>

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

/// Get sub-connectivity from the supplied connectivity and indices
///
/// @param element_connect The connectivity of the element
/// @param idxs The indices to extract
/// @return The sub-connectivity
std::vector<std::size_t> sub_connect(const std::vector<std::size_t> & element_connect,
                                     const std::vector<int> & idxs);

/// Create a key from the supplied indices. Use this to construct keys for edges and faces
///
/// @param idxs The indices to create a key from
/// @return The key
std::vector<std::int64_t> key(const std::vector<std::size_t> & idxs);

/// Get map keys
template <typename K, typename V>
std::vector<K>
map_keys(const std::map<K, V> & in_map)
{
    std::vector<K> keys;
    for (const auto & pair : in_map) {
        keys.push_back(pair.first);
    }
    return keys;
}

template <typename T>
int64_t
index_of(const std::vector<T> & vec, const T & value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end())
        return std::distance(vec.begin(), it);
    else
        return -1; // Value not found
}

} // namespace krado::utils
