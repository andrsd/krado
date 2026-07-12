// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/document_file.h"
#include <vector>
#include <string>
#include <filesystem>

namespace krado {

/// Class for reading IGES files
class IGESFile : public DocumentFile {
public:
    /// IGES file
    ///
    /// @param file_name Name of the file to work with
    explicit IGESFile(const std::filesystem::path & file_name);

    /// Write shapes into a file
    ///
    /// @param shapes Shapes to write
    void write(const std::vector<GeomShape> & shapes);

    /// Read the file
    ///
    /// @return Shapes that were contained in the IGES file
    std::vector<GeomShape> read() const;
};

} // namespace krado
