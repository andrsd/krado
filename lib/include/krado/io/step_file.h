#pragma once

#include <string>

namespace krado {

class STEPFile {
public:
    STEPFile();
    void load(const std::string & file_name);
};

} // namespace krado
