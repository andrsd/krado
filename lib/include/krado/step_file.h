// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include <string>

namespace krado {

/// Class for reading STEP files
class STEPFile {
public:
    STEPFile(const std::string & file_name);
    [[nodiscard]] GeomShape load() const;

private:
    std::string file_name_;
};

} // namespace krado
