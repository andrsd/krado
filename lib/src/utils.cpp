// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/utils.h"
#include <cstdint>

namespace krado::utils {

std::string
to_upper(const std::string & text)
{
    std::string upper(text);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

std::string
to_lower(const std::string & text)
{
    std::string lower(text);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

std::vector<std::size_t>
sub_connect(const std::vector<std::size_t> & element_connect, const std::vector<int> & idxs)
{
    std::vector<std::size_t> connect;
    for (auto i : idxs)
        connect.emplace_back(element_connect[i]);
    return connect;
}

std::vector<std::int64_t>
key(const std::vector<std::size_t> & idxs)
{
    std::vector<std::int64_t> k(idxs.begin(), idxs.end());
    std::sort(k.begin(), k.end());
    return k;
}

} // namespace krado::utils
