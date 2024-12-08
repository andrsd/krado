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
    std::tuple<std::vector<Element>, std::map<int, std::vector<gidx_t>>> read_elements();

    /// Read side sets
    ///
    /// @return Vector of side set entries
    std::map<int, std::vector<side_set_entry_t>> read_side_sets();

    /// Determine mesh spatial dimension
    ///
    /// @param mesh Mesh object
    /// @return Spatial dimension
    int determine_spatial_dim(const Mesh & mesh);

    /// Write info
    void write_info();

    /// Write coordinates
    void write_coords(const Mesh & mesh);

    /// Write elements
    void write_elements(const Mesh & mesh);

    /// Write side sets
    void write_side_sets(const Mesh & mesh);

    /// File name
    std::string fn;
    /// ExodusII file object
    exodusIIcpp::File exo;
    /// Spatial dimension
    int dim;
    /// Maaping from krado indexing into exodusii indexing
    std::map<std::size_t, int> exii_elem_ids;
};

} // namespace krado
