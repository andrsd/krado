// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/step_file.h"
#include "krado/exception.h"
#include "Interface_Static.hxx"
#include "STEPControl_Reader.hxx"

namespace krado {

STEPFile::STEPFile(const std::string & file_name) : file_name_(file_name) {}

std::vector<GeomShape>
STEPFile::load() const
{
    TCollection_AsciiString fname(this->file_name_.c_str());

    STEPControl_Reader reader;
    Interface_Static::SetIVal("read.step.ideas", 1);
    Interface_Static::SetIVal("read.step.nonmanifold", 1);
    if (reader.ReadFile(fname.ToCString()) != IFSelect_RetDone)
        throw Exception("Could not read file '{}'", this->file_name_);

    reader.NbRootsForTransfer();
    reader.TransferRoots();
    std::vector<GeomShape> shapes;
    for (int idx = 1; idx <= reader.NbShapes(); ++idx)
        shapes.push_back(GeomShape(-1, reader.Shape(idx)));
    return shapes;
}

} // namespace krado
