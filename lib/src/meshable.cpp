// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/meshable.h"

namespace krado {

Meshable::Meshable() : meshed_(false), marker_(0) {}

bool
Meshable::is_meshed() const
{
    return this->meshed_;
}

void
Meshable::set_meshed()
{
    this->meshed_ = true;
}

void
Meshable::set_marker(marker_t marker)
{
    this->marker_ = marker;
}

marker_t
Meshable::marker() const
{
    return this->marker_;
}

} // namespace krado
