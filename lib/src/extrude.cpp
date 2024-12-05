// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/extrude.h"
#include "krado/element.h"
#include "krado/point.h"

namespace krado {

namespace {

template <Element::Type T>
Element
extrude_element(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    throw Exception("Extrusion of element type '{}' not supported", Element::type(T));
}

template <>
Element
extrude_element<Element::LINE2>(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    std::array<std::size_t, Quad4::N_VERTICES> ids;
    ids[0] = el(0) + layer * layer_stride;
    ids[1] = el(1) + layer * layer_stride;
    ids[2] = el(1) + (layer + 1) * layer_stride;
    ids[3] = el(0) + (layer + 1) * layer_stride;
    return Element::Quad4(ids, el.marker());
}

template <>
Element
extrude_element<Element::TRI3>(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    std::array<std::size_t, Prism6::N_VERTICES> ids;
    ids[0] = el(0) + layer * layer_stride;
    ids[1] = el(1) + layer * layer_stride;
    ids[2] = el(2) + layer * layer_stride;
    ids[3] = el(0) + (layer + 1) * layer_stride;
    ids[4] = el(1) + (layer + 1) * layer_stride;
    ids[5] = el(2) + (layer + 1) * layer_stride;
    return Element::Prism6(ids, el.marker());
}

template <>
Element
extrude_element<Element::QUAD4>(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    std::array<std::size_t, Hex8::N_VERTICES> ids;
    ids[0] = el(0) + layer * layer_stride;
    ids[1] = el(1) + layer * layer_stride;
    ids[2] = el(2) + layer * layer_stride;
    ids[3] = el(3) + layer * layer_stride;
    ids[4] = el(0) + (layer + 1) * layer_stride;
    ids[5] = el(1) + (layer + 1) * layer_stride;
    ids[6] = el(2) + (layer + 1) * layer_stride;
    ids[7] = el(3) + (layer + 1) * layer_stride;
    return Element::Hex8(ids, el.marker());
}

} // namespace

Mesh
extrude(const Mesh & mesh, const Vector & normal, int layers, double thickness)
{
    std::vector<double> thicknesses(layers, thickness / layers);
    return extrude(mesh, normal, thicknesses);
}

Mesh
extrude(const Mesh & mesh, const Vector & direction, const std::vector<double> & thicknesses)
{
    auto n = direction.normalized();
    auto layer_stride = mesh.points().size();

    std::vector<Point> points;
    points.reserve(layer_stride * (thicknesses.size() + 1));
    for (auto & pt : mesh.points())
        points.emplace_back(pt);
    for (std::size_t i = 0; i < thicknesses.size(); ++i) {
        auto delta = thicknesses[i];
        for (auto & pt : mesh.points()) {
            auto p = pt + (i + 1) * n * delta;
            points.emplace_back(p);
        }
    }

    std::vector<Element> elems;
    for (std::size_t i = 0; i < thicknesses.size(); ++i) {
        for (auto & el : mesh.elements()) {
            if (el.type() == Element::LINE2)
                elems.emplace_back(extrude_element<Element::LINE2>(el, i, layer_stride));
            else if (el.type() == Element::TRI3)
                elems.emplace_back(extrude_element<Element::TRI3>(el, i, layer_stride));
            else if (el.type() == Element::QUAD4)
                elems.emplace_back(extrude_element<Element::QUAD4>(el, i, layer_stride));
            else
                throw Exception("Extrusion of element type '{}' not supported",
                                Element::type(el.type()));
        }
    }

    std::map<marker_t, std::vector<std::size_t>> set_cell_sets;
    for (std::size_t id = 0; id < elems.size(); ++id) {
        auto & el = elems[id];
        auto marker = el.marker();
        set_cell_sets[marker].push_back(id);
    }

    Mesh extruded_mesh(points, elems);
    for (auto & id : mesh.cell_set_ids())
        extruded_mesh.set_cell_set_name(id, mesh.cell_set_name(id));
    return extruded_mesh;
}

} // namespace krado
