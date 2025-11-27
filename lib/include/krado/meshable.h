// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"

namespace krado {

/// Interface for entities that can be meshed
class Meshable {
public:
    Meshable();

    /// Check if the curve is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark curve as meshed
    void set_meshed();

    /// Set entity marker
    ///
    /// @param marker Marker to assign to the entity
    void set_marker(marker_t marker);

    /// Get entity marker
    ///
    /// @return Marker associated with this entity
    marker_t marker() const;

private:
    /// Flag indicating if the entity is meshed
    bool meshed_;
    ///
    marker_t marker_;
};

} // namespace krado
