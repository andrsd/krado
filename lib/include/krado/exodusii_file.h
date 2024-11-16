// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include <string>

namespace krado {

class ExodusIIFile {
public:
    /// ExodusIIFile constructor
    ///
    /// @param file_name Name of the ExodusII file
    explicit ExodusIIFile(const std::string & file_name);

    /// Read mesh from ExodusII file
    ///
    /// @return Mesh object read from file
    Mesh read();

private:
    /// Read nodes
    ///
    /// @return Vector of points
    std::vector<Point> read_points();

    /// Read elements
    ///
    /// @return Vector of elements
    std::vector<MeshElement> read_elements();

    /// File name
    std::string fn;
    /// ExodusII file object
    exodusIIcpp::File exo;
};

} // namespace krado
