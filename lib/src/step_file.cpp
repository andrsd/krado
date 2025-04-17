// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/step_file.h"
#include "krado/exception.h"
#include "krado/geom_shape.h"
#include "TDocStd_Document.hxx"
#include "STEPCAFControl_Reader.hxx"
#include "XCAFDoc.hxx"
#include "XCAFDoc_Material.hxx"
#include "XCAFDoc_DocumentTool.hxx"
#include "XCAFDoc_ShapeTool.hxx"
#include "TDataStd_TreeNode.hxx"

namespace krado {

STEPFile::STEPFile(const std::string & file_name) : file_name_(file_name) {}

std::vector<GeomShape>
STEPFile::load() const
{
    std::vector<GeomShape> shapes;

    TCollection_AsciiString fname(this->file_name_.c_str());
    STEPCAFControl_Reader reader;
    reader.SetNameMode(Standard_True);
    reader.SetMatMode(Standard_True);
    Handle(TDocStd_Document) doc = new TDocStd_Document(TCollection_ExtendedString("formo-doc"));
    if (!reader.Perform(fname, doc))
        throw Exception("Failed to load STEP file");

    auto shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    TDF_LabelSequence shape_labels;
    shape_tool->GetShapes(shape_labels);

    for (Standard_Integer i = 1; i <= shape_labels.Length(); ++i) {
        auto shape_label = shape_labels.Value(i);
        auto shape = XCAFDoc_ShapeTool::GetShape(shape_label);
        GeomShape geom_shape(-1, shape);

        Handle(TDataStd_TreeNode) node;
        if (shape_label.FindAttribute(XCAFDoc::MaterialRefGUID(), node) && node->HasFather()) {
            const TDF_Label material_label = node->Father()->Label();
            Handle(XCAFDoc_Material) mat;
            if (material_label.FindAttribute(XCAFDoc_Material::GetID(), mat)) {
                geom_shape.set_material(mat->GetName()->ToCString(), mat->GetDensity());
            }
        }

        shapes.push_back(geom_shape);
    }

    return shapes;
}

} // namespace krado
