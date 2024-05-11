// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace krado::utils {

/**
 * Convert supplied string to upper case.
 * @param text The string to convert upper case.
 */
std::string to_upper(const std::string & text);

/**
 * Convert supplied string to lower case.
 * @param text The string to convert upper case.
 */
std::string to_lower(const std::string & text);

} // namespace krado::utils
