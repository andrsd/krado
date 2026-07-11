// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/iges_file.h"
#include "krado/exception.h"
#include "IGESControl_Reader.hxx"
#include "IGESCAFControl_Writer.hxx"
#include "TDocStd_Document.hxx"

namespace krado {

IGESFile::IGESFile(const std::filesystem::path & file_name) : DocumentFile(file_name) {}

void
IGESFile::write(const std::vector<GeomShape> & shapes)
{
    Handle(TDocStd_Document) doc = create_doc(shapes);

    IGESCAFControl_Writer writer;
    writer.SetNameMode(true);
    if (writer.Transfer(doc)) {
        writer.Write(file_name().c_str());
    }
}

std::vector<GeomShape>
IGESFile::read() const
{
    IGESControl_Reader reader;
    if (reader.ReadFile(file_name().c_str()) != IFSelect_RetDone)
        throw Exception("Unable to load '{}'", file_name());
    reader.NbRootsForTransfer();
    reader.TransferRoots();
    std::vector<GeomShape> shapes;
    shapes.reserve(reader.NbShapes());
    for (int idx = 1; idx <= reader.NbShapes(); ++idx)
        shapes.push_back(GeomShape(reader.Shape(idx)));
    return shapes;
}

} // namespace krado
