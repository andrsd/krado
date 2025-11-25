// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/extrude.h"
#include "krado/element.h"
#include "krado/mesh.h"
#include "krado/point.h"
#include "krado/log.h"
#include "krado/utils.h"

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
    int dim = -1;
    std::vector<Element> elems;
    for (std::size_t i = 0; i < thicknesses.size(); ++i) {
        for (auto & el : mesh.elements()) {
            if (el.type() == ElementType::LINE2) {
                elems.emplace_back(extrude_element<ElementType::LINE2>(el, i, point_stride));
                dim = 1;
            }
            else if (el.type() == ElementType::TRI3) {
                elems.emplace_back(extrude_element<ElementType::TRI3>(el, i, point_stride));
                dim = 2;
            }
            else if (el.type() == ElementType::QUAD4) {
                elems.emplace_back(extrude_element<ElementType::QUAD4>(el, i, point_stride));
                dim = 2;
            }
            else
                throw Exception("Extrusion of element type '{}' not supported",
                                Element::type(el.type()));
        }
    }

    std::map<marker_t, std::vector<side_set_entry_t>> side_sets;
    if (dim == 1) {
        for (const auto & id : mesh.vertex_set_ids())
            side_sets[id] = utils::create_side_set(mesh, mesh.vertex_set(id));
    }
    else if (dim == 2) {
        for (const auto & id : mesh.edge_set_ids())
            side_sets[id] = utils::create_side_set(mesh, mesh.edge_set(id));
    }

    Mesh extruded_mesh(points, elems);
    extruded_mesh.set_up();
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
    if (dim == 1) {
        for (auto & [id, ss] : side_sets) {
            std::vector<side_set_entry_t> extruded_side_sets;
            extruded_side_sets.reserve(ss.size() * thicknesses.size());
            for (std::size_t i = 0; i < thicknesses.size(); ++i) {
                for (auto & entry : ss) {
                    auto cell_id = entry.elem + elem_stride * i;
                    auto & cell = mesh.element(entry.elem);
                    if (cell.type() == ElementType::LINE2) {
                        extruded_side_sets.emplace_back(
                            cell_id,
                            extrude_element_side<ElementType::LINE2>(cell, entry.side));
                    }
                    else
                        throw Exception("Extrusion of element type '{}' not supported in 1D",
                                        Element::type(cell.type()));
                }
            }
            extruded_mesh.set_edge_set(id,
                                       utils::set_from_side_set(extruded_mesh, extruded_side_sets));
            extruded_mesh.set_edge_set_name(id, mesh.vertex_set_name(id));
        }
    }
    else if (dim == 2) {
        for (auto & [id, ss] : side_sets) {
            std::vector<side_set_entry_t> extruded_side_sets;
            extruded_side_sets.reserve(ss.size() * thicknesses.size());
            for (std::size_t i = 0; i < thicknesses.size(); ++i) {
                for (auto & entry : ss) {
                    auto cell_id = entry.elem + elem_stride * i;
                    auto & cell = mesh.element(entry.elem);
                    if (cell.type() == ElementType::TRI3)
                        extruded_side_sets.emplace_back(
                            cell_id,
                            extrude_element_side<ElementType::TRI3>(cell, entry.side));
                    else if (cell.type() == ElementType::QUAD4)
                        extruded_side_sets.emplace_back(
                            cell_id,
                            extrude_element_side<ElementType::QUAD4>(cell, entry.side));
                    else
                        throw Exception("Extrusion of element type '{}' not supported",
                                        Element::type(cell.type()));
                }
            }
            extruded_mesh.set_face_set(id,
                                       utils::set_from_side_set(extruded_mesh, extruded_side_sets));
            extruded_mesh.set_face_set_name(id, mesh.edge_set_name(id));
        }
    }

    return extruded_mesh;
}

} // namespace krado
