// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh.h"
#include "krado/geom_model.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include <string>
#include <vector>

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

    /// Write mesh to ExodusII file
    ///
    /// @param model GeomModel object to write
    void write(const GeomModel & model);

private:
    /// Read nodes
    ///
    /// @return Vector of points
    [[nodiscard]] std::vector<Point> read_points();

    /// Read elements
    ///
    /// @return Vector of elements
    [[nodiscard]] std::tuple<std::vector<Element>, std::map<int, std::vector<Index>>>
    read_elements();

    /// Read side sets
    ///
    /// @param elems Vector of elements
    /// @return Vector of side set entries
    [[nodiscard]] std::map<int, std::vector<SideEntry>>
    read_side_sets(const std::vector<Element> & elems);

    /// Read node sets
    [[nodiscard]] std::map<int, std::vector<int>> read_node_sets();

    /// File name
    std::string fn_;
    /// ExodusII file object
    exodusIIcpp::File exo_;
};

} // namespace krado
