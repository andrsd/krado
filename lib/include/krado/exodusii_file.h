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
    [[nodiscard]] Mesh read();

    /// Write mesh to ExodusII file
    ///
    /// @param mesh Mesh object to write
    void write(const Mesh & mesh);

private:
    /// Read nodes
    ///
    /// @return Vector of points
    [[nodiscard]] std::vector<Point> read_points();

    /// Read elements
    ///
    /// @return Vector of elements
    [[nodiscard]] std::tuple<std::vector<Element>, std::map<int, std::vector<gidx_t>>>
    read_elements();

    /// Read side sets
    ///
    /// @param elems Vector of elements
    /// @return Vector of side set entries
    [[nodiscard]] std::map<int, std::vector<side_set_entry_t>>
    read_side_sets(const std::vector<Element> & elems);

    /// Determine mesh spatial dimension
    ///
    /// @param mesh Mesh object
    /// @return Spatial dimension
    [[nodiscard]] int determine_spatial_dim(const Mesh & mesh);

    /// Write info
    void write_info();

    /// Write coordinates
    void write_coords(const Mesh & mesh);

    /// Write elements
    void write_elements(const Mesh & mesh);

    /// Write side sets
    void write_side_sets(const Mesh & mesh);

    /// File name
    std::string fn_;
    /// ExodusII file object
    exodusIIcpp::File exo_;
    /// Spatial dimension
    int dim_;
    /// Maaping from krado indexing into exodusii indexing
    std::map<std::size_t, int> exii_elem_ids_;
};

} // namespace krado
