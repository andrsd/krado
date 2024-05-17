// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace krado {

class Mesh;

/// Write a mesh into an ExodusII file
///
/// @param mesh Mesh to write
/// @param file_name Name of the file
void write_exodusii(const Mesh & mesh, const std::string & file_name);

} // namespace krado
