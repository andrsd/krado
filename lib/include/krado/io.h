#pragma once

#include "krado/mesh.h"
#include <vector>

namespace krado {

class IO {
public:
    /// Write mesh into a file
    ///
    /// @param file_name Name of the file
    /// @param mesh Mesh to write
    static void export_mesh(const std::string & file_name, const Mesh & mesh);

    /// Read mesh from a file
    ///
    /// @param file_name Name of the file
    /// @return Mesh read from the file
    [[nodiscard]] static Mesh import_mesh(const std::string & file_name);
};

} // namespace krado
