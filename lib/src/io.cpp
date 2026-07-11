// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/io.h"
#include "krado/exodusii_file.h"

namespace krado {

void
IO::export_mesh(Ptr<const Mesh> mesh, const std::string & file_name)
{
    try {
        ExodusIIFile file(file_name);
        file.write(mesh);
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to write '{}'.", file_name);
    }
}

Ptr<Mesh>
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
