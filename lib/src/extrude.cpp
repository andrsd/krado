// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/extrude.h"
#include "krado/element.h"
#include "krado/mesh.h"
#include "krado/point.h"
#include "krado/log.h"

namespace krado {

namespace {

template <ElementType T>
Element
extrude_element(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    throw Exception("Extrusion of element type '{}' not supported", Element::type(T));
}

template <>
Element
extrude_element<ElementType::LINE2>(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    std::array<std::size_t, Quad4::N_VERTICES> ids;
    ids[0] = el(0) + layer * layer_stride;
    ids[1] = el(1) + layer * layer_stride;
    ids[2] = el(1) + (layer + 1) * layer_stride;
    ids[3] = el(0) + (layer + 1) * layer_stride;
    return Element::Quad4(ids);
}

template <>
Element
extrude_element<ElementType::TRI3>(const Element & el, std::size_t layer, std::size_t layer_stride)
{
    std::array<std::size_t, Prism6::N_VERTICES> ids;
    ids[0] = el(0) + layer * layer_stride;
    ids[1] = el(1) + layer * layer_stride;
    ids[2] = el(2) + layer * layer_stride;
    ids[3] = el(0) + (layer + 1) * layer_stride;
    ids[4] = el(1) + (layer + 1) * layer_stride;
    ids[5] = el(2) + (layer + 1) * layer_stride;
    return Element::Prism6(ids);
}

template <>
Element
extrude_element<ElementType::QUAD4>(const Element & el, std::size_t layer, std::size_t layer_stride)
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
    return Element::Hex8(ids);
}

//

template <ElementType T>
int
extrude_element_side(const Element & el, int side)
{
    throw Exception("Extrusion of element side for '{}' not supported", Element::type(T));
}

template <>
int
extrude_element_side<ElementType::LINE2>(const Element & el, int side)
{
    // this maps edge side to quad side
    std::array<int, 2> quad_side = { 3, 1 };
    return quad_side[side];
}

template <>
int
extrude_element_side<ElementType::TRI3>(const Element & el, int side)
{
    // this maps triangle side to prism side
    return side + 1;
}

template <>
int
extrude_element_side<ElementType::QUAD4>(const Element & el, int side)
{
    // this maps quad side to hex side
    std::array<int, 4> hex_side = { 0, 3, 1, 2 };
    return hex_side[side];
}

} // namespace

Mesh
extrude(const Mesh & mesh, const Vector & normal, int layers, double thickness)
{
    Log::info("Extruding mesh: normal={}, layers={}, thickness={}", normal, layers, thickness);

    std::vector<double> thicknesses(layers, thickness / layers);
    return extrude(mesh, normal, thicknesses);
}

Mesh
extrude(const Mesh & mesh, const Vector & direction, const std::vector<double> & thicknesses)
{
    auto n = direction.normalized();
    auto point_stride = mesh.points().size();
    auto elem_stride = mesh.elements().size();

    // extrude elements
    std::vector<Point> points;
    points.reserve(point_stride * (thicknesses.size() + 1));
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
            if (el.type() == ElementType::LINE2)
                elems.emplace_back(extrude_element<ElementType::LINE2>(el, i, point_stride));
            else if (el.type() == ElementType::TRI3)
                elems.emplace_back(extrude_element<ElementType::TRI3>(el, i, point_stride));
            else if (el.type() == ElementType::QUAD4)
                elems.emplace_back(extrude_element<ElementType::QUAD4>(el, i, point_stride));
            else
                throw Exception("Extrusion of element type '{}' not supported",
                                Element::type(el.type()));
        }
    }

    Mesh extruded_mesh(points, elems);
    // extrude cell sets
    for (auto & id : mesh.cell_set_ids()) {
        auto & cells = mesh.cell_set(id);
        std::vector<gidx_t> cell_set;
        cell_set.reserve(cells.size() * thicknesses.size());
        for (std::size_t i = 0; i < thicknesses.size(); ++i) {
            for (auto & cell : cells)
                cell_set.push_back(cell + elem_stride * i);
        }
        extruded_mesh.set_cell_set(id, cell_set);
        extruded_mesh.set_cell_set_name(id, mesh.cell_set_name(id));
    }
    // extrude side sets
    std::map<marker_t, std::vector<side_set_entry_t>> side_sets;
    for (auto & id : mesh.side_set_ids()) {
        auto & ss = mesh.side_set(id);
        std::vector<side_set_entry_t> side_set;
        side_set.reserve(ss.size() * thicknesses.size());
        for (std::size_t i = 0; i < thicknesses.size(); ++i) {
            for (auto & entry : ss) {
                auto cell_id = entry.elem + elem_stride * i;
                auto & cell = mesh.element(entry.elem);
                if (cell.type() == ElementType::LINE2)
                    side_set.emplace_back(
                        cell_id,
                        extrude_element_side<ElementType::LINE2>(cell, entry.side));
                else if (cell.type() == ElementType::TRI3)
                    side_set.emplace_back(
                        cell_id,
                        extrude_element_side<ElementType::TRI3>(cell, entry.side));
                else if (cell.type() == ElementType::QUAD4)
                    side_set.emplace_back(
                        cell_id,
                        extrude_element_side<ElementType::QUAD4>(cell, entry.side));
                else
                    throw Exception("Extrusion of element type '{}' not supported",
                                    Element::type(cell.type()));
            }
        }
        extruded_mesh.set_side_set(id, side_set);
        extruded_mesh.set_side_set_name(id, mesh.side_set_name(id));
    }
    return extruded_mesh;
}

} // namespace krado
