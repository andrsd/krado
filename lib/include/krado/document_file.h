// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include <vector>
#include <string>

class TDocStd_Document;
class XSControl_Reader;

namespace krado {

/// This class houses common functionality for reading/writing CAD files
class DocumentFile {
public:
    /// Create a file object (no I/O done)
    ///
    /// @param file_name Name of the file to read
    explicit DocumentFile(const std::filesystem::path & file_name);

    virtual ~DocumentFile() = default;

    /// Get file name
    const std::string file_name() const;

protected:
    TDocStd_Document * create_doc(const std::vector<GeomShape> & shapes);

private:
    std::string fname_;
};

} // namespace krado
