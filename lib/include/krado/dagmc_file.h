// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace krado {

class GeomModel;

/// Class for DAGMC files
class DAGMCFile {
public:
    DAGMCFile(const std::string & file_name);

    void write(const GeomModel & model);

private:
    std::string file_name_;
};

} // namespace krado
