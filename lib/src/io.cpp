// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/io.h"
#include "krado/exodusii_file.h"
#include "krado/step_file.h"
#include "krado/iges_file.h"
#include "krado/utils.h"
#include <filesystem>

namespace krado {

void
IO::export_mesh(Ptr<const Mesh> mesh, const std::filesystem::path & file_name)
{
    try {
        ExodusIIFile file(file_name);
        file.write(mesh);
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to write '{}'.", file_name.string());
    }
}

Ptr<Mesh>
IO::import_mesh(const std::filesystem::path & file_name)
{
    try {
        ExodusIIFile file(file_name);
        return file.read();
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to read '{}'.", file_name.string());
    }
}

void
IO::export_geometry(const std::vector<GeomShape> & shapes, const std::filesystem::path & file_name)
{
    auto ext = utils::to_lower(file_name.extension());
    if (ext == ".step" || ext == ".stp") {
        STEPFile file(file_name);
        file.write(shapes);
    }
    else if (ext == ".iges" || ext == ".igs") {
        IGESFile file(file_name);
        file.write(shapes);
    }
    else
        throw Exception("Unsupported file type '{}'.", file_name.string());
}

std::vector<GeomShape>
IO::import_geometry(const std::filesystem::path & file_name)
{
    auto ext = utils::to_lower(file_name.extension());
    if (ext == ".step" || ext == ".stp") {
        STEPFile file(file_name);
        return file.read();
    }
    else if (ext == ".iges" || ext == ".igs") {
        IGESFile file(file_name);
        return file.read();
    }
    else
        throw Exception("Unsupported file type '{}'.", file_name.string());
}

} // namespace krado
