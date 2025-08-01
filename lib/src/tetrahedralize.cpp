// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/tetrahedralize.h"
#include "krado/exception.h"
#include "krado/types.h"
#include "krado/element.h"
#include "krado/log.h"
#include <vector>
#include <array>

namespace krado {

// This is a rewritte from [1]
//
// [1] https://github.com/idaholab/moose/framework/include/utils/MooseMeshElementConversionUtils.h

namespace {

template <ElementType ET>
std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
nodes_to_tet_nodes_determiner(const std::array<gidx_t, ElementSelector<ET>::N_VERTICES> & el);

/// Split a given element into a series of TET4 elements
///
/// @param el The element to be split
/// @return A vector of TET4 elements
template <ElementType ET>
std::vector<std::array<gidx_t, Tetra4::N_VERTICES>>
elem_splitter(const std::array<gidx_t, ElementSelector<ET>::N_VERTICES> & el)
{
    auto optimized_node_list = nodes_to_tet_nodes_determiner<ET>(el);
    std::vector<std::array<gidx_t, Tetra4::N_VERTICES>> tet4_elems;
    for (size_t i = 0; i < optimized_node_list.size(); i++) {
        std::array<gidx_t, Tetra4::N_VERTICES> new_elem;
        for (gidx_t j = 0; j < Tetra4::N_VERTICES; j++)
            new_elem[j] = el[optimized_node_list[i][j]];
        tet4_elems.push_back(new_elem);
    }
    return tet4_elems;
}

/// Split a given element into a series of TET4 elements
///
/// @param elem The element to be split
/// @return A vector of TET4 elements
template <ElementType ET>
std::vector<std::array<gidx_t, Tetra4::N_VERTICES>>
split_elem(const Element & elem)
{
    auto connect = utils::to_array<ElementSelector<ET>::N_VERTICES>(elem.ids().cbegin());
    return elem_splitter<ET>(connect);
}

/// Get the number of TET4 elements that a given element will be split into
///
/// @param type The type of the element
/// @return The number of TET4 elements
int
num_of_tets(ElementType type)
{
    if (type == ElementType::HEX8)
        return 6;
    else if (type == ElementType::PRISM6)
        return 3;
    else if (type == ElementType::PYRAMID5)
        return 2;
    else
        return 1;
}

/// Calculate the indices (within the element nodes) of the three neighboring nodes of a node in a
/// HEX8 element.
///
/// @param min_id_index The index of the node with the minimum id
/// @return a vector of the three neighboring nodes
std::array<lidx_t, Tri3::N_VERTICES>
neighbor_node_indices_hex8(lidx_t min_id_index)
{
    const std::vector<std::array<lidx_t, Tri3::N_VERTICES>> preset_indices = {
        { 1, 3, 4 }, { 0, 2, 5 }, { 3, 1, 6 }, { 2, 0, 7 },
        { 5, 7, 0 }, { 4, 6, 1 }, { 7, 5, 2 }, { 6, 4, 3 }
    };
    if (min_id_index < Hex8::N_VERTICES)
        return preset_indices[min_id_index];
    else
        throw Exception("The input node index is out of range.");
}

bool
quad_face_diagonal_direction(const std::array<gidx_t, Quad4::N_VERTICES> & quad)
{
    auto min_id_index =
        std::distance(std::begin(quad), std::min_element(std::begin(quad), std::end(quad)));
    if (min_id_index == 0 || min_id_index == 2)
        return true;
    else
        return false;
}

std::array<bool, Hex8::N_FACES>
hex8_face_diagonal_directions(const std::array<gidx_t, Hex8::N_VERTICES> & hex)
{
    // Bottom/Top; Front/Back; Right/Left
    const std::vector<std::array<lidx_t, Quad4::N_VERTICES>> face_indices = {
        { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 0, 1, 5, 4 },
        { 2, 3, 7, 6 }, { 1, 2, 6, 5 }, { 3, 0, 4, 7 }
    };
    std::array<bool, Hex8::N_FACES> diagonal_directions;
    for (lidx_t i = 0; i < Hex8::N_FACES; i++) {
        auto & face_index = face_indices[i];
        std::array<gidx_t, Quad4::N_VERTICES> quad = { hex[face_index[0]],
                                                       hex[face_index[1]],
                                                       hex[face_index[2]],
                                                       hex[face_index[3]] };
        diagonal_directions[i] = quad_face_diagonal_direction(quad);
    }
    return diagonal_directions;
}

//

std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
tet4_nodes_for_hex8(const std::array<bool, Hex8::N_FACES> & diagonal_directions)
{
    const std::vector<std::array<bool, Hex8::N_FACES>> possible_inputs = {
        { true, true, true, true, true, false },   { true, true, true, true, false, false },
        { true, true, true, false, true, false },  { true, false, true, true, true, false },
        { true, false, true, true, false, false }, { true, false, true, false, true, false },
        { true, false, true, false, false, false }
    };

    auto input_index = std::distance(
        std::begin(possible_inputs),
        std::find(std::begin(possible_inputs), std::end(possible_inputs), diagonal_directions));

    switch (input_index) {
    case 0:
        return { { 0, 1, 2, 6 }, { 0, 5, 1, 6 }, { 0, 4, 5, 6 },
                 { 0, 2, 3, 7 }, { 0, 6, 2, 7 }, { 0, 4, 6, 7 } };
    case 1:
        return { { 0, 1, 2, 5 }, { 0, 2, 6, 5 }, { 0, 6, 4, 5 },
                 { 0, 2, 3, 7 }, { 0, 6, 2, 7 }, { 0, 4, 6, 7 } };
    case 2:
        return { { 0, 1, 2, 6 }, { 0, 5, 1, 6 }, { 0, 4, 5, 6 },
                 { 0, 7, 4, 6 }, { 0, 3, 7, 6 }, { 0, 2, 3, 6 } };
    case 3:
        return { { 0, 7, 4, 5 }, { 0, 6, 7, 5 }, { 0, 1, 6, 5 },
                 { 0, 3, 7, 2 }, { 0, 7, 6, 2 }, { 0, 6, 1, 2 } };

    case 4:
        // NOTE: This case supplies only 5 variants for faces and nodes (why?)
        return { { 0, 1, 2, 5 }, { 0, 2, 3, 7 }, { 4, 7, 5, 0 }, { 5, 7, 6, 2 }, { 0, 2, 7, 5 } };

    case 5:
        return { { 0, 7, 4, 5 }, { 0, 6, 7, 5 }, { 0, 1, 6, 5 },
                 { 1, 6, 2, 0 }, { 2, 6, 3, 0 }, { 3, 6, 7, 0 } };
    case 6:
        return { { 0, 4, 5, 7 }, { 0, 5, 6, 7 }, { 0, 6, 3, 7 },
                 { 0, 5, 1, 2 }, { 0, 6, 5, 2 }, { 0, 3, 6, 2 } };
    default:
        throw Exception("Unexpected input.");
    }
}

/// Rotate a HEX8 element's nodes to ensure that the node with the minimum id is the first node;
/// and the node among its three neighboring nodes with the minimum id is the second node.
///
/// @param min_id_index The index of the node with the minimum id
/// @param sec_min_pos The index of the node among its three neighboring nodes with the minimum
///        id (see comments in the function for more details about how the index is defined)
/// @return node_rotation - A vector of node indices that can form a HEX8 element
std::array<lidx_t, Hex8::N_VERTICES>
hex8_rotation(lidx_t min_id_index, lidx_t sec_min_pos)
{
    // Assuming the original hex element is a cube, the vectors formed by nodes 0-1, 0-2, and 0-4
    // are overlapped with the x, y, and z axes, respectively. sec_min_pos = 0 means the second
    // minimum node is in the x direction, sec_min_pos = 1 means the second minimum node is in the y
    // direction, and sec_min_pos = 2 means the second minimum node is in the z direction.
    const std::vector<std::vector<std::array<lidx_t, Hex8::N_VERTICES>>> node_indices = {
        { { 0, 1, 2, 3, 4, 5, 6, 7 }, { 0, 3, 7, 4, 1, 2, 6, 5 }, { 0, 4, 5, 1, 3, 7, 6, 2 } },
        { { 1, 0, 4, 5, 2, 3, 7, 6 }, { 1, 2, 3, 0, 5, 6, 7, 4 }, { 1, 5, 6, 2, 0, 4, 7, 3 } },
        { { 2, 3, 0, 1, 6, 7, 4, 5 }, { 2, 1, 5, 6, 3, 0, 4, 7 }, { 2, 6, 7, 3, 1, 5, 4, 0 } },
        { { 3, 2, 6, 7, 0, 1, 5, 4 }, { 3, 0, 1, 2, 7, 4, 5, 6 }, { 3, 7, 4, 0, 2, 6, 5, 1 } },
        { { 4, 5, 1, 0, 7, 6, 2, 3 }, { 4, 7, 6, 5, 0, 3, 2, 1 }, { 4, 0, 3, 7, 5, 1, 2, 6 } },
        { { 5, 4, 7, 6, 1, 0, 3, 2 }, { 5, 6, 2, 1, 4, 7, 3, 0 }, { 5, 1, 0, 4, 6, 2, 3, 7 } },
        { { 6, 7, 3, 2, 5, 4, 0, 1 }, { 6, 5, 4, 7, 2, 1, 0, 3 }, { 6, 2, 1, 5, 7, 3, 0, 4 } },
        { { 7, 6, 5, 4, 3, 2, 1, 0 }, { 7, 4, 0, 3, 6, 5, 1, 2 }, { 7, 3, 2, 6, 4, 0, 1, 5 } }
    };

    if (min_id_index < Hex8::N_VERTICES && sec_min_pos < 3)
        return node_indices[min_id_index][sec_min_pos];
    else
        throw Exception("The input node index is out of range.");
}

template <>
std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
nodes_to_tet_nodes_determiner<ElementType::HEX8>(const std::array<gidx_t, Hex8::N_VERTICES> & hex)
{
    // Find the node with the minimum id
    auto min_node_id_index =
        std::distance(std::begin(hex), std::min_element(std::begin(hex), std::end(hex)));
    // Get the index of the three neighbor nodes of the minimum node
    // The order is consistent with the description in nodeRotationHEX8()
    // Then determine the index of the second minimum node
    auto neighbor_node_indices = neighbor_node_indices_hex8(min_node_id_index);

    auto neighbor_node_ids = { hex[neighbor_node_indices[0]],
                               hex[neighbor_node_indices[1]],
                               hex[neighbor_node_indices[2]] };
    auto sec_min_pos =
        std::distance(std::begin(neighbor_node_ids),
                      std::min_element(std::begin(neighbor_node_ids), std::end(neighbor_node_ids)));

    // Rotate the node and face indices based on the identified minimum and second minimum nodes
    // After the rotation, we guarantee that the minimum node is the first node (Node 0)
    // And the second node (Node 1) has the minium global id among the three neighbor nodes of Node
    // 0 This makes the splitting process simpler
    auto node_rotation = hex8_rotation(min_node_id_index, sec_min_pos);

    std::array<gidx_t, Hex8::N_VERTICES> rotated_hex_nodes;
    for (lidx_t i = 0; i < Hex8::N_VERTICES; i++)
        rotated_hex_nodes[i] = hex[node_rotation[i]];

    // Find the selection of each face's cutting direction
    auto diagonal_directions = hex8_face_diagonal_directions(rotated_hex_nodes);

    // Based on the determined splitting directions of all the faces, determine the nodes of each
    // resulting TET4 elements after the splitting.
    auto tet_nodes_set = tet4_nodes_for_hex8(diagonal_directions);

    std::vector<std::array<lidx_t, Tetra4::N_VERTICES>> tet_nodes_list;
    for (const auto & tet_nodes : tet_nodes_set)
        tet_nodes_list.push_back(tet_nodes);
    return tet_nodes_list;
}

// PYRAMID5

std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
tet4_nodes_for_pyramid5()
{
    const std::vector<std::array<lidx_t, Tetra4::N_VERTICES>> node_indices = { { 0, 1, 2, 4 },
                                                                               { 0, 2, 3, 4 } };
    return node_indices;
}

std::array<lidx_t, Pyramid5::N_VERTICES>
pyramid5_rotation(gidx_t min_id_index)
{
    const std::vector<std::array<lidx_t, Pyramid5::N_VERTICES>> node_indices = {
        { 0, 1, 2, 3, 4 },
        { 1, 2, 3, 0, 4 },
        { 2, 3, 0, 1, 4 },
        { 3, 0, 1, 2, 4 }
    };

    if (min_id_index <= 3)
        return node_indices[min_id_index];
    else
        throw Exception("The input node index is out of range.");
}

template <>
std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
nodes_to_tet_nodes_determiner<ElementType::PYRAMID5>(
    const std::array<gidx_t, Pyramid5::N_VERTICES> & pyramid)
{
    auto min_node_id_index =
        std::distance(std::begin(pyramid),
                      std::min_element(std::begin(pyramid), std::end(pyramid)));

    // Rotate the node and face indices based on the identified minimum nodes
    // After the rotation, we guarantee that the minimum node is the first node (Node 0)
    // This makes the splitting process simpler
    auto node_rotation = pyramid5_rotation(min_node_id_index);

    std::array<gidx_t, Pyramid5::N_VERTICES> rotated_pyramid_nodes;
    for (lidx_t i = 0; i < Pyramid5::N_VERTICES; i++)
        rotated_pyramid_nodes[i] = pyramid[node_rotation[i]];

    // There is only one quad face in a pyramid element, so the splitting selection is binary
    auto tet_nodes_set = tet4_nodes_for_pyramid5();

    std::vector<std::array<lidx_t, Tetra4::N_VERTICES>> tet_nodes_list;
    for (const auto & tet_nodes : tet_nodes_set)
        tet_nodes_list.push_back(tet_nodes);
    return tet_nodes_list;
}

// PRISM6

/// Creates sets of four nodes indices that can form TET4 elements to replace the original PRISM6
/// element.
///
/// @param diagonal_direction A boolean value indicating the direction of the diagonal line of
///        face 2
/// @return A vector of vectors of node indices that can form TET4 elements
std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
tet4_nodes_for_prism6(bool diagonal_direction)
{
    if (diagonal_direction) {
        return { { 3, 5, 4, 0 }, { 1, 4, 5, 0 }, { 1, 5, 2, 0 } };
    }
    else {
        return { { 3, 5, 4, 0 }, { 1, 4, 2, 0 }, { 2, 4, 5, 0 } };
    }
}

/// Rotate a PRISM6 element nodes to ensure that the node with the minimum id is the first node.
///
/// @param min_id_index The index of the node, within the prism nodes, with the minimum id
/// @return A vector of node indices that can form a PRISM6 element
std::array<lidx_t, Prism6::N_VERTICES>
prism6_rotation(lidx_t min_id_index)
{
    const std::vector<std::array<lidx_t, Prism6::N_VERTICES>> node_indices = {
        { 0, 1, 2, 3, 4, 5 }, { 1, 2, 0, 4, 5, 3 }, { 2, 0, 1, 5, 3, 4 },
        { 3, 5, 4, 0, 2, 1 }, { 4, 3, 5, 1, 0, 2 }, { 5, 4, 3, 2, 1, 0 }
    };

    if (min_id_index <= 5)
        return node_indices[min_id_index];
    else
        throw std::runtime_error("The input node index is out of range.");
}

template <>
std::vector<std::array<lidx_t, Tetra4::N_VERTICES>>
nodes_to_tet_nodes_determiner<ElementType::PRISM6>(
    const std::array<gidx_t, Prism6::N_VERTICES> & prism)
{
    auto min_node_id_index =
        std::distance(std::begin(prism), std::min_element(std::begin(prism), std::end(prism)));

    // Rotate the node and face indices based on the identified minimum node
    // After the rotation, we guarantee that the minimum node is the first node (Node 0)
    // This makes the splitting process simpler
    auto node_rotation = prism6_rotation(min_node_id_index);

    std::array<gidx_t, Prism6::N_VERTICES> rotated_prism_nodes;
    for (lidx_t i = 0; i < Prism6::N_VERTICES; i++)
        rotated_prism_nodes[i] = prism[node_rotation[i]];

    std::array<gidx_t, Quad4::N_VERTICES> key_quad_nodes = { rotated_prism_nodes[1],
                                                             rotated_prism_nodes[2],
                                                             rotated_prism_nodes[5],
                                                             rotated_prism_nodes[4] };

    // Find the selection of each face's cutting direction
    auto diagonal_direction = quad_face_diagonal_direction(key_quad_nodes);

    // Based on the determined splitting directions of all the faces, determine the nodes of each
    // resulting TET4 elements after the splitting.
    auto tet_nodes_set = tet4_nodes_for_prism6(diagonal_direction);

    std::vector<std::array<lidx_t, Tetra4::N_VERTICES>> tet_nodes_list;
    for (const auto & tet_nodes : tet_nodes_set)
        tet_nodes_list.push_back(tet_nodes);
    return tet_nodes_list;
}

} // namespace

Mesh
tetrahedralize(const Mesh & mesh)
{
    Log::info("Tetrahedralizing mesh");

    std::vector<Point> points = mesh.points();

    gid_t n_tets = 0;
    for (auto & el : mesh.elements())
        n_tets += num_of_tets(el.type());

    std::map<gidx_t, std::vector<gidx_t>> elem_map;
    std::vector<Element> elems;
    elems.reserve(n_tets);
    for (gidx_t cell_id = 0; cell_id < mesh.elements().size(); ++cell_id) {
        auto & el = mesh.element(cell_id);
        if (el.type() == ElementType::HEX8) {
            auto tet4s = split_elem<ElementType::HEX8>(el);
            for (auto & tet : tet4s) {
                elem_map[cell_id].push_back(elems.size());
                elems.emplace_back(Element::Tetra4(tet));
            }
        }
        else if (el.type() == ElementType::PYRAMID5) {
            auto tet4s = split_elem<ElementType::PYRAMID5>(el);
            for (auto & tet : tet4s) {
                elem_map[cell_id].push_back(elems.size());
                elems.emplace_back(Element::Tetra4(tet));
            }
        }
        else if (el.type() == ElementType::PRISM6) {
            auto tet4s = split_elem<ElementType::PRISM6>(el);
            for (auto & tet : tet4s) {
                elem_map[cell_id].push_back(elems.size());
                elems.emplace_back(Element::Tetra4(tet));
            }
        }
        else {
            elem_map[cell_id].push_back(elems.size());
            elems.push_back(el);
        }
    }

    Mesh tet_mesh(points, elems);
    for (auto id : mesh.cell_set_ids()) {
        auto & cells = mesh.cell_set(id);
        std::vector<gidx_t> new_cell_set;
        for (auto & cell_id : cells) {
            auto & tet_elems = elem_map[cell_id];
            new_cell_set.insert(new_cell_set.end(), tet_elems.begin(), tet_elems.end());
        }
        tet_mesh.set_cell_set(id, new_cell_set);
        tet_mesh.set_cell_set_name(id, mesh.cell_set_name(id));
    }
    return tet_mesh;
}

} // namespace krado
