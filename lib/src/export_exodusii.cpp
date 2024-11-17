// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/export.h"
#include "krado/mesh.h"
#include "krado/exception.h"
#include "krado/exodusii_file.h"

namespace krado {

void
write_exodusii(const Mesh & mesh, const std::string & file_name)
{
    try {
        ExodusIIFile file(file_name);
        file.write(mesh);
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to write '{}'.", file_name);
    }
}

} // namespace krado
