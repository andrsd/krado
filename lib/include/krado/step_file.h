#pragma once

#include "krado/geom_shape.h"
#include <string>

namespace krado {

/// Class for reading STEP files
class STEPFile {
public:
    STEPFile(const std::string & file_name);
    geo::GeomShape load() const;

private:
    std::string file_name;
};

} // namespace krado
