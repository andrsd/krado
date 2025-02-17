// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"

namespace krado {

class MeshVertexAbstract {
public:
    MeshVertexAbstract();
    virtual ~MeshVertexAbstract() = default;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] virtual Point point() const = 0;

    /// Get global ID
    ///
    /// @return Global ID of this vertex
    [[nodiscard]] int global_id() const;

    /// Set global ID
    ///
    /// @param id New ID to assign
    void set_global_id(int id);

private:
    int gid_;
};

} // namespace krado
