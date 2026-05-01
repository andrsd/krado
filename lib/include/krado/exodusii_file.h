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
    /// File name
    std::string fn_;
    /// ExodusII file object
    exodusIIcpp::File exo_;
};

} // namespace krado
