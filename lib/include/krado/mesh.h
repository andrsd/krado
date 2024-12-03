// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/point.h"
#include "krado/scheme_factory.h"
#include "krado/bounding_box_3d.h"
#include "krado/transform.h"
#include "krado/hasse_diagram.h"
#include "krado/utils.h"
#include <cstdint>
#include <map>

namespace krado {

struct side_set_entry_t {
    std::size_t elem;
    std::size_t side;

    side_set_entry_t(std::size_t elem, std::size_t side) : elem(elem), side(side) {}
};

class GeomModel;

class Mesh {
public:
    /// Construct empty mesh
    Mesh();

    /// Construct mesh from geometric model
    ///
    /// @param model Geometric model
    Mesh(const GeomModel & model);

    /// Construct mesh from set of points and elements (useful for reading meshes from files)
    ///
    /// @param points Points
    /// @param elements Elements
    Mesh(std::vector<Point> points, std::vector<Element> elements);

    /// Vertex
    const MeshVertex & vertex(int id) const;
    MeshVertex & vertex(int id);

    /// Vertices
    const std::map<int, MeshVertex> & vertices() const;

    /// Curve
    const MeshCurve & curve(int id) const;
    MeshCurve & curve(int id);

    /// Curves
    const std::map<int, MeshCurve> & curves() const;

    /// Surface
    const MeshSurface & surface(int id) const;
    MeshSurface & surface(int id);

    /// Surfaces
    const std::map<int, MeshSurface> & surfaces() const;

    /// Volume
    const MeshVolume & volume(int id) const;
    MeshVolume & volume(int id);

    /// Volumes
    const std::map<int, MeshVolume> & volumes() const;

    /// Create vertex mesh
    void mesh_vertex(int id);
    void mesh_vertex(MeshVertex & vertex);

    /// Create curve mesh
    void mesh_curve(int id);
    void mesh_curve(MeshCurve & curve);

    /// Create surface mesh
    void mesh_surface(int id);
    void mesh_surface(MeshSurface & surface);

    /// Create volume mesh
    void mesh_volume(int id);
    void mesh_volume(MeshVolume & volume);

    /// Get mesh points
    ///
    /// @return Mesh points
    const std::vector<Point> & points() const;

    /// Get a point by index
    ///
    /// @param idx Index of the point
    /// @return Point
    const Point & point(std::size_t idx) const;

    /// Get elements
    ///
    /// @return Mesh elements
    const std::vector<Element> & elements() const;

    /// Get element of the mesh
    ///
    /// @param idx Index of the element
    /// @return Element
    const Element & element(std::size_t idx) const;

    ///
    void number_points();

    ///
    void build_elements();

    /// Get mesh bounding box
    BoundingBox3D bounding_box() const;

    /// Scale mesh by a factor (isotropic)
    ///
    /// @param factor Scaling factor
    /// @return Scaled mesh
    Mesh scaled(double factor) const;

    /// Scale mesh by a factor (unisotropic)
    ///
    /// @param factor_x Scaling factor in x-direction
    /// @param factor_y Scaling factor in y-direction
    /// @param factor_z Scaling factor in z-direction
    /// @return Scaled mesh
    Mesh scaled(double factor_x, double factor_y, double factor_z = 1.) const;

    /// Translate mesh
    ///
    /// @param tx Translation in x-direction
    /// @param ty Translation in y-direction
    /// @param tz Translation in z-direction
    /// @return Translated mesh
    Mesh translated(double tx, double ty = 0., double tz = 0.) const;

    /// Transform mesh
    ///
    /// @param tr Transformation
    /// @return Transformed mesh
    Mesh transformed(const Trsf & tr) const;

    /// Add another mesh to this mesh
    ///
    /// @param other Mesh to add
    void add(const Mesh & other);

    /// Remove duplicate points
    ///
    /// @param tolerance Tolerance for point comparison
    void remove_duplicate_points(double tolerance = 1e-12);

    /// Compute bounding box around the mesh
    ///
    /// @return Bounding box
    BoundingBox3D compute_bounding_box() const;

    /// Duplicate mesh
    ///
    /// @return Duplicated mesh
    Mesh duplicate() const;

    /// Set cell set name
    ///
    /// @param cell_set_id Cell set ID (marker)
    /// @param name Cell set name
    void set_cell_set_name(marker_t cell_set_id, const std::string & name);

    /// Get cell set name
    ///
    /// @param cell_set_id Cell set ID (marker)
    /// @return Cell set name
    std::string cell_set_name(marker_t cell_set_id) const;

    /// Get cell set IDs
    std::vector<marker_t> cell_set_ids() const;

    /// Get cell set
    ///
    /// @param id Cell set ID
    /// @return Cell set
    const std::vector<std::size_t> cell_set(marker_t id) const;

    /// Set cell set
    ///
    /// @param id Cell set ID
    /// @param cell_ids Cell IDs
    void set_cell_set(marker_t id, const std::vector<std::size_t> cell_ids);

    /// Set face set name
    ///
    /// @param face_set_id Face set ID (marker)
    /// @param name Face set name
    void set_face_set_name(marker_t face_set_id, const std::string & name);

    /// Get face set name
    ///
    /// @param face_set_id Face set ID (marker)
    /// @return Face set name
    std::string face_set_name(marker_t cell_set_id) const;

    /// Get face set IDs
    std::vector<marker_t> face_set_ids() const;

    /// Get face set
    ///
    /// @param id Face set ID
    /// @return Face set
    const std::vector<std::size_t> face_set(marker_t id) const;

    /// Set face set
    ///
    /// @param id Face set ID
    /// @param face_ids Face IDs
    void set_face_set(marker_t id, const std::vector<std::size_t> face_ids);

    /// Set edge set name
    ///
    /// @param edge_set_id Edge set ID (marker)
    /// @param name Edge set name
    void set_edge_set_name(marker_t edge_set_id, const std::string & name);

    /// Get edge set name
    ///
    /// @param edge_set_id Edge set ID (marker)
    /// @return Edge set name
    std::string edge_set_name(marker_t edge_set_id) const;

    /// Get edge set IDs
    std::vector<marker_t> edge_set_ids() const;

    /// Get edge set
    ///
    /// @param id Edge set ID
    /// @return Edge set
    const std::vector<std::size_t> edge_set(marker_t id) const;

    /// Set edge set
    ///
    /// @param id Edge set ID
    /// @param edge_ids Edge IDs
    void set_edge_set(marker_t id, const std::vector<std::size_t> edge_ids);

    /// Set side set name
    ///
    /// @param id Side set ID
    /// @param name Side set name
    void set_side_set_name(marker_t id, const std::string & name);

    /// Get side set name
    ///
    /// @param id Side set ID
    /// @return Side set name
    std::string side_set_name(marker_t id) const;

    /// Get side set IDs
    std::vector<marker_t> side_set_ids() const;

    /// Get side set
    ///
    /// @param id Side set ID
    /// @return Side set
    const std::vector<side_set_entry_t> side_set(marker_t id) const;

    /// Set side set
    ///
    /// @param id Side set ID
    /// @param elem_ids Element IDs
    void set_side_set(marker_t id, const std::vector<std::size_t> elem_ids);

    /// Remap block IDs
    ///
    /// @param block_map Map of old block IDs to new block IDs
    void remap_block_ids(const std::map<marker_t, marker_t> & block_map);

    /// Get mesh point IDs
    ///
    /// @return Mesh point IDs
    const std::set<std::size_t> & point_ids() const;

    /// Get mesh edge IDs
    ///
    /// @return Mesh edge IDs
    const std::set<std::size_t> & edge_ids() const;

    /// Get mesh face IDs
    ///
    /// @return Mesh face IDs
    const std::set<std::size_t> & face_ids() const;

    /// Get mesh cell IDs
    ///
    /// @return Mesh cell IDs
    const std::set<std::size_t> & cell_ids() const;

    /// Get support of a mesh node
    ///
    /// @param index Index of the node
    const std::vector<std::size_t> & support(int64_t index) const;

    /// Get connectivity of a mesh node
    ///
    /// @param index Index of the node
    const std::vector<std::size_t> & connectivity(int64_t index) const;

    /// Get element type
    ///
    /// @param index Index of the element
    /// @return Element type
    Element::Type element_type(int64_t index) const;

    /// Prepare mesh
    void set_up();

    /// Get all boundary edge IDs
    ///
    /// @return Boundary edge IDs
    std::vector<std::size_t> boundary_edges() const;

    /// Get all boundary face IDs
    ///
    /// @return Boundary face IDs
    std::vector<std::size_t> boundary_faces() const;

    /// Compute centroid
    ///
    /// @param index Index of the element
    /// @return Centroid
    Point compute_centroid(std::size_t index) const;

    /// Compute outward normal
    ///
    /// @param index Index of the edge/face
    /// @return Outward normal
    Vector outward_normal(std::size_t index) const;

protected:
    void build_1d_elements();
    void build_2d_elements();

    template <typename T, typename U>
    T &
    get_scheme(U entity) const
    {
        return dynamic_cast<T &>(entity.scheme());
    }

    /// Assign a new global ID to a vertex
    ///
    /// @param vertex Vertex to assign global ID to
    void assign_gid(MeshVertex & vertex);

    /// Assign a new global ID to a curve vertex
    ///
    /// @param vertex Curve vertex to assign global ID to
    void assign_gid(MeshCurveVertex & vertex);

    /// Assign a new global ID to a surface vertex
    ///
    /// @param vertex Surface vertex to assign global ID to
    void assign_gid(MeshSurfaceVertex & vertex);

    void add_mesh_point(Point & mpnt);

private:
    void initialize(const GeomModel & model);

    void build_hasse_diagram();

    void
    hasse_add_edge(std::size_t parent_node_id, const std::vector<std::size_t> & edge_connect)
    {
        auto k = utils::key(edge_connect);
        if (this->key_map.find(k) == this->key_map.end()) {
            auto edge_node_id = this->hasse.nodes.size();
            this->key_map[k] = edge_node_id;
            this->hasse.add_node(edge_node_id, HasseDiagram::Node::Edge);
            this->hasse.add_edge(parent_node_id, edge_node_id);
            this->elems.emplace_back(Element::Line2({ edge_connect[0], edge_connect[1] }));
        }
        else {
            auto edge_node_id = this->key_map[k];
            this->hasse.add_edge(parent_node_id, edge_node_id);
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_faces(std::size_t id, const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = this->key_map[iid];

        const auto & elem_connect = elem.ids();
        for (std::size_t j = 0; j < ELEMENT_TYPE::N_FACES; ++j) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[j]);
            auto k = utils::key(face_connect);
            if (this->key_map.find(k) == this->key_map.end()) {
                auto face_node_id = this->hasse.nodes.size();
                this->key_map[k] = face_node_id;
                this->hasse.add_node(face_node_id, HasseDiagram::Node::Face);
                this->hasse.add_edge(elem_node_id, face_node_id);
                if (face_connect.size() == 3)
                    this->elems.emplace_back(
                        Element::Tri3({ face_connect[0], face_connect[1], face_connect[2] }));
                else if (face_connect.size() == 4)
                    this->elems.emplace_back(Element::Quad4(
                        { face_connect[0], face_connect[1], face_connect[2], face_connect[3] }));
            }
            else {
                auto face_node_id = this->key_map[k];
                this->hasse.add_edge(elem_node_id, face_node_id);
            }
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_face_edges(std::size_t id, const Element & elem)
    {
        auto iid = utils::key(-(id + 1));

        const auto & elem_connect = elem.ids();
        for (std::size_t i = 0; i < ELEMENT_TYPE::N_FACES; ++i) {
            auto face_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::FACE_VERTICES[i]);
            auto face_node_id = this->key_map[utils::key(face_connect)];

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
    hasse_add_edges(std::size_t id, const Element & elem)
    {
        auto iid = utils::key(-(id + 1));
        auto elem_node_id = this->key_map[iid];

        const auto & elem_connect = elem.ids();
        for (std::size_t j = 0; j < ELEMENT_TYPE::N_EDGES; ++j) {
            auto edge_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            hasse_add_edge(elem_node_id, edge_connect);
        }
    }

    template <class ELEMENT_TYPE>
    void
    hasse_add_edge_vertices(std::size_t id, const Element & elem)
    {
        const auto & elem_connect = elem.ids();
        for (std::size_t j = 0; j < ELEMENT_TYPE::N_EDGES; ++j) {
            auto edge_connect = utils::sub_connect(elem_connect, ELEMENT_TYPE::EDGE_VERTICES[j]);
            auto edge_node_id = this->key_map[utils::key(edge_connect)];
            for (auto & vtx : edge_connect) {
                auto vtx_id = utils::key(vtx);
                if (this->key_map.find(vtx_id) != this->key_map.end()) {
                    auto vtx_node_id = this->key_map[vtx_id];
                    this->hasse.add_edge(edge_node_id, vtx_node_id);
                }
                else
                    throw Exception("Vertex not found in key map");
            }
        }
    }

    std::map<int, MeshVertex> vtxs;
    std::map<int, MeshCurve> crvs;
    std::map<int, MeshSurface> surfs;
    std::map<int, MeshVolume> vols;

    SchemeFactory & scheme_factory;

    /// Mesh points
    std::vector<Point> pnts;
    /// All mesh elements. Point, edge, face, and cell IDs are indexing into this container.
    std::vector<Element> elems;
    /// Cell set names
    std::map<marker_t, std::string> cell_set_names;
    /// Cell sets
    std::map<marker_t, std::vector<std::size_t>> cell_sets;
    /// Face set names
    std::map<marker_t, std::string> face_set_names;
    /// Face sets
    std::map<marker_t, std::vector<std::size_t>> face_sets;
    /// Edge set names
    std::map<marker_t, std::string> edge_set_names;
    /// Edge sets
    std::map<marker_t, std::vector<std::size_t>> edge_sets;

    /// Side set names
    std::map<marker_t, std::string> side_set_names;
    /// Side sets
    std::map<marker_t, std::vector<side_set_entry_t>> side_sets;

    /// Global ID counter
    int gid_ctr;
    /// Bounding box around the mesh that is being exported
    BoundingBox3D exp_bbox;
    /// Hasse diagram representing the mesh
    HasseDiagram hasse;
    /// Map of keys to node IDs
    std::map<std::vector<std::int64_t>, std::size_t> key_map;
};

} // namespace krado
