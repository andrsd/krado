// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/bounding_box_3d.h"
#include "krado/element.h"
#include "krado/exception.h"
#include "krado/point.h"
#include "krado/transform.h"
#include "krado/range.h"
#include "krado/hasse_diagram.h"
#include "krado/utils.h"
#include <cstdint>
#include <map>

namespace krado {

/// Side set entry
struct side_set_entry_t {
    /// Element ID
    gidx_t elem;
    /// Local side number
    std::size_t side;

    side_set_entry_t(gidx_t elem, std::size_t side) : elem(elem), side(side) {}
};

/// Equality operator for side set entry
inline bool
operator==(const side_set_entry_t & lhs, const side_set_entry_t & rhs)
{
    return lhs.elem == rhs.elem && lhs.side == rhs.side;
}

/// Class representing a mesh
///
/// A mesh is a collection of points and elements. Each element is a collection of points.
/// A point has 3 coordinates (x, y, z).
/// Cell sets, face sets, and edge sets can be defined on the mesh.
/// Boundary edges and faces can be computed and obtained via API.
/// Simple operations like scaling, and translation are supported. General transformations
/// can be applied to the mesh as well. They are based on homogeneous coordinates and supported
/// by `Trsf` class.
///
/// This class should be used for procedural mesh generation.
class Mesh {
public:
    /// Construct empty mesh
    Mesh();

    /// Construct mesh from set of points and elements (useful for reading meshes from files)
    ///
    /// @param points Points
    /// @param elements Elements
    Mesh(std::vector<Point> points, std::vector<Element> elements);

    /// Get mesh points
    ///
    /// @return Mesh points
    [[nodiscard]] const std::vector<Point> & points() const;

    /// Get a point by index
    ///
    /// @param idx Index of the point
    /// @return Point
    [[nodiscard]] const Point & point(gidx_t idx) const;

    /// Get elements
    ///
    /// @return Mesh elements
    [[nodiscard]] const std::vector<Element> & elements() const;

    /// Get element of the mesh
    ///
    /// @param idx Index of the element
    /// @return Element
    [[nodiscard]] const Element & element(gidx_t idx) const;

    /// Scale mesh by a factor (isotropic)
    ///
    /// @param factor Scaling factor
    /// @return Scaled mesh
    [[nodiscard]] Mesh scaled(double factor) const;

    /// Scale mesh by a factor (isotropic)
    ///
    /// @param factor Scaling factor
    /// @return Reference to this mesh which is scaled
    Mesh & scale(double factor);

    /// Scale mesh by a factor (unisotropic)
    ///
    /// @param factor_x Scaling factor in x-direction
    /// @param factor_y Scaling factor in y-direction
    /// @param factor_z Scaling factor in z-direction
    /// @return Scaled mesh
    [[nodiscard]] Mesh scaled(double factor_x, double factor_y, double factor_z = 1.) const;

    /// Scale mesh by a factor (unisotropic)
    ///
    /// @param factor_x Scaling factor in x-direction
    /// @param factor_y Scaling factor in y-direction
    /// @param factor_z Scaling factor in z-direction
    /// @return Reference to this mesh which is scaled
    Mesh & scale(double factor_x, double factor_y, double factor_z = 1.);

    /// Translate mesh
    ///
    /// @param tx Translation in x-direction
    /// @param ty Translation in y-direction
    /// @param tz Translation in z-direction
    /// @return Translated mesh
    [[nodiscard]] Mesh translated(double tx, double ty = 0., double tz = 0.) const;

    /// Translate mesh
    ///
    /// @param tx Translation in x-direction
    /// @param ty Translation in y-direction
    /// @param tz Translation in z-direction
    /// @return Reference to this mesh which is translated
    Mesh & translate(double tx, double ty = 0., double tz = 0.);

    /// Transform mesh
    ///
    /// @param tr Transformation
    /// @return Transformed mesh
    [[nodiscard]] Mesh transformed(const Trsf & tr) const;

    /// Transform mesh
    ///
    /// @param tr Transformation
    /// @return Reference to this mesh which is transformed
    Mesh & transform(const Trsf & tr);

    /// Add another mesh to this mesh
    ///
    /// @param other Mesh to add
    /// @return Reference to this mesh
    Mesh & add(const Mesh & other);

    /// Remove duplicate points
    ///
    /// @param tolerance Tolerance for point comparison
    /// @return Reference to this mesh
    Mesh & remove_duplicate_points(double tolerance = 1e-12);

    /// Compute bounding box around the mesh
    ///
    /// @return Bounding box
    [[nodiscard]] BoundingBox3D compute_bounding_box() const;

    /// Duplicate mesh
    ///
    /// @return Duplicated mesh
    [[nodiscard]] Mesh duplicate() const;

    /// Set cell set name
    ///
    /// @param cell_set_id Cell set ID (marker)
    /// @param name Cell set name
    /// @return Reference to this mesh
    Mesh & set_cell_set_name(marker_t cell_set_id, const std::string & name);

    /// Get cell set name
    ///
    /// @param cell_set_id Cell set ID (marker)
    /// @return Cell set name
    [[nodiscard]] std::string cell_set_name(marker_t cell_set_id) const;

    /// Get cell set IDs
    [[nodiscard]] std::vector<marker_t> cell_set_ids() const;

    /// Get cell set
    ///
    /// @param id Cell set ID
    /// @return Cell set
    [[nodiscard]] const std::vector<gidx_t> & cell_set(marker_t id) const;

    /// Set cell set
    ///
    /// @param id Cell set ID
    /// @param cell_ids Cell IDs
    /// @return Reference to this mesh
    Mesh & set_cell_set(marker_t id, const std::vector<gidx_t> & cell_ids);

    /// Set face set name
    ///
    /// @param face_set_id Face set ID (marker)
    /// @param name Face set name
    /// @return Reference to this mesh
    Mesh & set_face_set_name(marker_t face_set_id, const std::string & name);

    /// Get face set name
    ///
    /// @param face_set_id Face set ID (marker)
    /// @return Face set name
    [[nodiscard]] std::string face_set_name(marker_t cell_set_id) const;

    /// Get face set IDs
    [[nodiscard]] std::vector<marker_t> face_set_ids() const;

    /// Get face set
    ///
    /// @param id Face set ID
    /// @return Face set
    [[nodiscard]] const std::vector<gidx_t> & face_set(marker_t id) const;

    /// Set face set
    ///
    /// @param id Face set ID
    /// @param face_ids Face IDs
    /// @return Reference to this mesh
    Mesh & set_face_set(marker_t id, const std::vector<gidx_t> & face_ids);

    /// Remove face sets
    ///
    /// @return Reference to this mesh
    Mesh & remove_face_sets();

    /// Set edge set name
    ///
    /// @param edge_set_id Edge set ID (marker)
    /// @param name Edge set name
    /// @return Reference to this mesh
    Mesh & set_edge_set_name(marker_t edge_set_id, const std::string & name);

    /// Get edge set name
    ///
    /// @param edge_set_id Edge set ID (marker)
    /// @return Edge set name
    [[nodiscard]] std::string edge_set_name(marker_t edge_set_id) const;

    /// Get edge set IDs
    [[nodiscard]] std::vector<marker_t> edge_set_ids() const;

    /// Get edge set
    ///
    /// @param id Edge set ID
    /// @return Edge set
    [[nodiscard]] const std::vector<gidx_t> & edge_set(marker_t id) const;

    /// Set edge set
    ///
    /// @param id Edge set ID
    /// @param edge_ids Edge IDs
    /// @return Reference to this mesh
    Mesh & set_edge_set(marker_t id, const std::vector<gidx_t> & edge_ids);

    /// Remove edge sets
    ///
    /// @return Reference to this mesh
    Mesh & remove_edge_sets();

    /// Set side set name
    ///
    /// @param id Side set ID
    /// @param name Side set name
    /// @return Reference to this mesh
    Mesh & set_side_set_name(marker_t id, const std::string & name);

    /// Get side set name
    ///
    /// @param id Side set ID
    /// @return Side set name
    [[nodiscard]] std::string side_set_name(marker_t id) const;

    /// Get side set IDs
    [[nodiscard]] std::vector<marker_t> side_set_ids() const;

    /// Get side set
    ///
    /// @param id Side set ID
    /// @return Side set
    [[nodiscard]] const std::vector<side_set_entry_t> & side_set(marker_t id) const;

    /// Set side set
    ///
    /// @param id Side set ID
    /// @param elem_ids Element IDs
    /// @return Reference to this mesh
    Mesh & set_side_set(marker_t id, const std::vector<gidx_t> & elem_ids);

    /// Set side set
    ///
    /// @param id Side set ID
    /// @param side_set_entries Side set entries
    /// @return Reference to this mesh
    Mesh & set_side_set(marker_t id, const std::vector<side_set_entry_t> & side_set_entries);

    /// Remove side sets
    ///
    /// @return Reference to this mesh
    Mesh & remove_side_sets();

    /// Remap block IDs
    ///
    /// @param block_map Map of old block IDs to new block IDs
    /// @return Reference to this mesh
    Mesh & remap_block_ids(const std::map<marker_t, marker_t> & block_map);

    /// Get mesh point IDs
    ///
    /// @return Mesh point IDs
    [[nodiscard]] const Range & point_ids() const;

    /// Get mesh edge IDs
    ///
    /// @return Mesh edge IDs
    [[nodiscard]] const Range & edge_ids() const;

    /// Get mesh face IDs
    ///
    /// @return Mesh face IDs
    [[nodiscard]] const Range & face_ids() const;

    /// Get mesh cell IDs
    ///
    /// @return Mesh cell IDs
    [[nodiscard]] const Range & cell_ids() const;

    /// Get support of a mesh node
    ///
    /// @param index Index of the node
    [[nodiscard]] std::vector<gidx_t> support(gidx_t index) const;

    /// Get connectivity of a mesh node
    ///
    /// @param index Index of the node
    [[nodiscard]] std::vector<gidx_t> cone(gidx_t index) const;

    /// Get cone vertices of a mesh node
    ///
    /// @param index Index of the node
    /// @return Cone vertices
    [[nodiscard]] std::set<gidx_t> cone_vertices(gidx_t index) const;

    /// Get element type
    ///
    /// @param index Index of the element
    /// @return Element type
    [[nodiscard]] ElementType element_type(gidx_t index) const;

    /// Prepare mesh
    void set_up();

    /// Get all boundary edge IDs
    ///
    /// @return Boundary edge IDs
    [[nodiscard]] std::vector<gidx_t> boundary_edges() const;

    /// Get all boundary face IDs
    ///
    /// @return Boundary face IDs
    [[nodiscard]] std::vector<gidx_t> boundary_faces() const;

    /// Compute centroid
    ///
    /// @param index Index of the element
    /// @return Centroid
    [[nodiscard]] Point compute_centroid(gidx_t index) const;

    /// Compute outward normal
    ///
    /// @param index Index of the edge/face
    /// @return Outward normal
    [[nodiscard]] Vector outward_normal(gidx_t index) const;

private:
    void build_hasse_diagram();

    void
    hasse_add_edge(gidx_t parent_node_id, const std::vector<gidx_t> & edge_connect)
    {
        auto k = utils::key(edge_connect);
        if (this->key_map_.find(k) == this->key_map_.end()) {
            gidx_t edge_node_id = this->hasse_.size();
            this->key_map_[k] = edge_node_id;
            this->hasse_.add_node(edge_node_id, HasseDiagram::NodeType::Edge);
            this->hasse_.add_edge(parent_node_id, edge_node_id);
        }
        else {
            auto edge_node_id = this->key_map_[k];
            this->hasse_.add_edge(parent_node_id, edge_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_faces(gidx_t id, const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = this->key_map_[iid];

        const auto & elem_connect = elem.ids();
        for (std::size_t j = 0; j < ELEMENT_TYPE::N_FACES; ++j) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto k = utils::key(face_connect);
            if (this->key_map_.find(k) == this->key_map_.end()) {
                auto face_node_id = this->hasse_.size();
                this->key_map_[k] = face_node_id;
                this->hasse_.add_node(face_node_id, HasseDiagram::NodeType::Face);
                this->hasse_.add_edge(elem_node_id, face_node_id);
            }
            else {
                auto face_node_id = this->key_map_[k];
                this->hasse_.add_edge(elem_node_id, face_node_id);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_face_edges(gidx_t id, const Element & elem)
    {
        const auto & elem_connect = elem.ids();
        for (std::size_t i = 0; i < ELEMENT_TYPE::N_FACES; ++i) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[i]);
            auto face_node_id = this->key_map_[utils::key(face_connect)];

            for (std::size_t j = 0; j < ELEMENT_TYPE::FACE_EDGES[i].size(); ++j) {
                auto edge = ELEMENT_TYPE::FACE_EDGES[i][j];
                auto edge_connect =
                    utils::sub_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[edge]);
                hasse_add_edge(face_node_id, edge_connect);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_edges(gidx_t id, const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = this->key_map_[iid];

        const auto & elem_connect = elem.ids();
        for (std::size_t j = 0; j < ELEMENT_TYPE::N_EDGES; ++j) {
            auto edge_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            hasse_add_edge(elem_node_id, edge_connect);
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_edge_vertices(gidx_t id, const Element & elem)
    {
        const auto & elem_connect = elem.ids();
        for (std::size_t j = 0; j < ELEMENT_TYPE::N_EDGES; ++j) {
            auto edge_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto edge_node_id = this->key_map_[utils::key(edge_connect)];
            for (auto & vtx : edge_connect) {
                auto vtx_id = utils::key(vtx);
                if (this->key_map_.find(vtx_id) != this->key_map_.end()) {
                    auto vtx_node_id = this->key_map_[vtx_id];
                    this->hasse_.add_edge(edge_node_id, vtx_node_id);
                }
                else
                    throw Exception("Vertex not found in key map");
            }
        }
    }

    /// Mesh points
    std::vector<Point> pnts_;
    /// All mesh elements. Point, edge, face, and cell IDs are indexing into this container.
    std::vector<Element> elems_;
    /// Cell set names
    std::map<marker_t, std::string> cell_set_names_;
    /// Cell sets
    std::map<marker_t, std::vector<gidx_t>> cell_sets_;
    /// Face set names
    std::map<marker_t, std::string> face_set_names_;
    /// Face sets
    std::map<marker_t, std::vector<gidx_t>> face_sets_;
    /// Edge set names
    std::map<marker_t, std::string> edge_set_names_;
    /// Edge sets
    std::map<marker_t, std::vector<gidx_t>> edge_sets_;

    /// Side set names
    std::map<marker_t, std::string> side_set_names_;
    /// Side sets
    std::map<marker_t, std::vector<side_set_entry_t>> side_sets_;

    /// Hasse diagram representing the mesh
    HasseDiagram hasse_;
    /// Map of keys to node IDs
    std::map<std::size_t, gidx_t> key_map_;
};

} // namespace krado
