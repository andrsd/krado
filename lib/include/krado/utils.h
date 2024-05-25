// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include <cstdint>
#include <string>
#include <algorithm>
#include <vector>
#include <map>

namespace krado {
class Point;
}

namespace krado::utils {

/// Convert supplied string to upper case.
///
/// @param text The string to convert upper case.
[[nodiscard]] std::string to_upper(const std::string & text);

/// Convert supplied string to lower case.
///
/// @param text The string to convert upper case.
[[nodiscard]] std::string to_lower(const std::string & text);

/// Check that `value` is equal to one of the `options`
///
/// @return `true` if `value` is one of the `options`, `false` otherwise
/// @param value Value to test
/// @param options Possible options
template <typename T>
[[nodiscard]] inline bool
in(T value, const std::vector<T> & options)
{
    return std::any_of(options.cbegin(), options.cend(), [value](const T & o) {
        return value == o;
    });
}

template <>
[[nodiscard]] inline bool
in<std::string>(std::string value, const std::vector<std::string> & options)
{
    std::string v = utils::to_lower(value);
    return std::any_of(options.cbegin(), options.cend(), [v](const std::string & o) {
        return v == utils::to_lower(o);
    });
}

template <>
[[nodiscard]] inline bool
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
[[nodiscard]] std::vector<gidx_t> sub_connect(const std::vector<gidx_t> & element_connect,
                                              const std::vector<int> & idxs);

/// Create a key from the supplied index. Use this to construct keys for cells
///
/// @param id The index to create a key from
/// @return The key
[[nodiscard]] std::size_t key(const std::size_t id);

/// Create a key from the supplied indices. Use this to construct keys for edges and faces
///
/// @param idxs The indices to create a key from
/// @return The key
[[nodiscard]] std::size_t key(const std::vector<gidx_t> & idxs);

/// Get map keys
template <typename K, typename V>
[[nodiscard]] std::vector<K>
map_keys(const std::map<K, V> & in_map)
{
    std::vector<K> keys;
    for (const auto & pair : in_map) {
        keys.push_back(pair.first);
    }
    return keys;
}

template <typename T>
[[nodiscard]] int64_t
index_of(const std::vector<T> & vec, const T & value)
{
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end())
        return std::distance(vec.begin(), it);
    else
        return -1; // Value not found
}

// Conversion from `std::vector` to `std::array`
//
// Taken from: https://stackoverflow.com/a/40931342/6122323

template <typename T, typename Iter, std::size_t... Is>
[[nodiscard]] constexpr auto
to_array(Iter & iter, std::index_sequence<Is...>) -> std::array<T, sizeof...(Is)>
{
    return { { ((void) Is, *iter++)... } };
}

template <std::size_t N,
          typename Iter,
          typename T = typename std::iterator_traits<Iter>::value_type>
[[nodiscard]] constexpr auto
to_array(Iter iter) -> std::array<T, N>
{
    return to_array<T>(iter, std::make_index_sequence<N> {});
}

template <typename T>
std::string to_str(T val);

/// Compute the angle at (p2) between three points
///
/// @param p1 The first point
/// @param p2 The second point
/// @param p3 The third point
/// @return The angle at (p2) between the three points [radians]
[[nodiscard]] double angle(const Point & p1, const Point & p2, const Point & p3);

/// Compute the area of a triangle
///
/// @param p1 The first point
/// @param p2 The second point
/// @param p3 The third point
/// @return The area of the triangle
[[nodiscard]] double triangle_area(const Point & p1, const Point & p2, const Point & p3);

/// Compute the distance between two points
///
/// @param p1 The first point
/// @param p2 The second point
/// @return The distance between the two points
[[nodiscard]] double distance(const Point & p1, const Point & p2);

} // namespace krado::utils
