// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/io.h"
#include "krado/exodusii_file.h"
#include "krado/log.h"

namespace krado {

void
IO::export_mesh(const std::string & file_name, const Mesh & mesh)
{
    try {
        ExodusIIFile file(file_name);
        file.write(mesh);
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to write '{}'.", file_name);
    }
}

Mesh
IO::import_mesh(const std::string & file_name)
{
    try {
        ExodusIIFile file(file_name);
        return file.read();
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to read '{}'.", file_name);
    }
}

} // namespace krado
