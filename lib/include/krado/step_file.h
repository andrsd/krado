// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/document_file.h"
#include <string>
#include <vector>
#include <filesystem>

namespace krado {

/// Class for reading STEP files
class STEPFile : public DocumentFile {
public:
    STEPFile(const std::filesystem::path & file_name);

    /// Write shapes into a file
    ///
    /// @param shapes Shapes to write
    void write(const std::vector<GeomShape> & shapes);

    /// Read the file
    ///
    /// @return Shapes that were contained in the STEP file
    std::vector<GeomShape> read() const;
};

} // namespace krado
