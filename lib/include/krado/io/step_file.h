#pragma once

#include "krado/geo/model.h"
#include <string>

namespace krado {

/// Class for reading STEP files
class STEPFile {
public:
    STEPFile(const std::string & file_name);
    Model load() const;

private:
    std::string file_name;
};

} // namespace krado
