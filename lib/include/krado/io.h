// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh.h"
#include "krado/geom_shape.h"
#include <vector>

namespace krado {

class IO {
public:
    /// Write mesh into a file
    ///
    /// @param file_name Name of the file
    /// @param mesh Mesh to write
    static void export_mesh(Ptr<const Mesh> mesh, const std::filesystem::path & file_name);

    /// Read mesh from a file
    ///
    /// @param file_name Name of the file
    /// @return Mesh read from the file
    [[nodiscard]] static Ptr<Mesh> import_mesh(const std::filesystem::path & file_name);

    /// Write geometry into a file
    ///
    /// @param file_name Name of the file
    /// @param shapes Geometry to write
    static void export_geometry(const std::vector<GeomShape> & shapes,
                                const std::filesystem::path & file_name);

    /// Read geometry from a file
    ///
    /// @param file_name Name of the file
    /// @return Geometry read from the file
    [[nodiscard]] static std::vector<GeomShape>
    import_geometry(const std::filesystem::path & file_name);
};

} // namespace krado
