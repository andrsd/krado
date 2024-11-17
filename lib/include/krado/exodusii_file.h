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

    /// Write mesh to ExodusII file
    ///
    /// @param mesh Mesh object to write
    void write(const Mesh & mesh);

private:
    /// Read nodes
    ///
    /// @return Vector of points
    std::vector<Point> read_points();

    /// Read elements
    ///
    /// @return Vector of elements
    std::vector<Element> read_elements();

    /// Determine mesh spatial dimension
    ///
    /// @param mesh Mesh object
    /// @return Spatial dimension
    int determine_spatial_dim(const Mesh & mesh);

    /// Write info
    void write_info();

    /// Write coordinates
    void write_coords();

    /// Write elements
    void write_elements();

    /// Preprocess mesh for output
    void preprocess_mesh(const Mesh & mesh);

    /// File name
    std::string fn;
    /// ExodusII file object
    exodusIIcpp::File exo;
    /// Spatial dimension
    int dim;
    /// coordinates
    std::vector<double> x, y, z;
    /// Element blocks
    std::map<int, std::vector<Element>> elem_blks;
};

} // namespace krado
