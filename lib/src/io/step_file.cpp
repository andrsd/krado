#include "krado/io/step_file.h"
#include "krado/exception.h"
#include "Interface_Static.hxx"
#include "STEPControl_Reader.hxx"

namespace krado {

STEPFile::STEPFile(const std::string & file_name) : file_name(file_name) {}

geo::Shape
STEPFile::load() const
{
    TCollection_AsciiString fname(file_name.c_str());

    STEPControl_Reader reader;
    Interface_Static::SetIVal("read.step.ideas", 1);
    Interface_Static::SetIVal("read.step.nonmanifold", 1);
    if (reader.ReadFile(fname.ToCString()) != IFSelect_RetDone)
        throw Exception("Could not read file '{}'", file_name);

    reader.NbRootsForTransfer();
    reader.TransferRoots();
    auto result = reader.OneShape();
    return geo::Shape(result);
}

} // namespace krado
