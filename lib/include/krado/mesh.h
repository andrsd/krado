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
#include "krado/types.h"
#include "krado/utils.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <set>

namespace krado {

class GeomModel;

/// Class representing a mesh
///
/// A mesh is a collection of points and elements. Each element is a collection of points.
/// A point has 3 coordinates (x, y, z).
/// Cell sets, side sets, and node sets can be defined on the mesh.
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

    Mesh(const Mesh & mesh) = delete;
    Mesh & operator=(const Mesh & mesh) = delete;

    /// Get number of points
    ///
    /// @return Number of points
    std::size_t num_points() const;

    /// Get number of elements
    ///
    /// @return Number of elements
    std::size_t num_elements() const;

    /// Get mesh points
    ///
    /// @return Mesh points
    [[nodiscard]] Span<const Point> points() const;

    /// Get a point by index
    ///
    /// @param idx Index of the point
    /// @return Point
    [[nodiscard]] const Point & point(Index idx) const;

    /// Get elements
    ///
    /// @return Mesh elements
    [[nodiscard]] Span<const Element> elements() const;

    /// Get element of the mesh
    ///
    /// @param idx Index of the element
    /// @return Element
    [[nodiscard]] const Element & element(Index idx) const;

    /// Scale mesh by a factor (isotropic)
    ///
    /// @param factor Scaling factor
    /// @return Scaled mesh
    [[nodiscard]] Ptr<Mesh> scaled(double factor) const;

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
    [[nodiscard]] Ptr<Mesh> scaled(double factor_x, double factor_y, double factor_z = 1.) const;

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
    [[nodiscard]] Ptr<Mesh> translated(double tx, double ty = 0., double tz = 0.) const;

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
    [[nodiscard]] Ptr<Mesh> transformed(const Trsf & tr) const;

    /// Transform mesh
    ///
    /// @param tr Transformation
    /// @return Reference to this mesh which is transformed
    Mesh & transform(const Trsf & tr);

    /// Mirror the mesh via a "plane"
    ///
    /// @param axis Plane to use for mirroring
    /// @return Mirrored mesh
    Ptr<Mesh> mirrored(const Axis2 & axis) const;

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

    /// Duplicate mesh
    ///
    /// @return Duplicated mesh
    [[nodiscard]] Ptr<Mesh> duplicate() const;

    /// Set cell set name
    ///
    /// @param cell_set_id Cell set ID (marker)
    /// @param name Cell set name
    /// @return Reference to this mesh
    Mesh & set_cell_set_name(Marker cell_set_id, const std::string & name);

    /// Get cell set name
    ///
    /// @param cell_set_id Cell set ID (marker)
    /// @return Cell set name
    Optional<std::string> cell_set_name(Marker cell_set_id) const;

    /// Get cell set IDs
    [[nodiscard]] std::vector<Marker> cell_set_ids() const;

    /// Get cell set
    ///
    /// @param id Cell set ID
    /// @return Cell set
    [[nodiscard]] Span<const Index> cell_set(Marker id) const;

    /// Set cell set
    ///
    /// @param id Cell set ID
    /// @param cell_ids Cell IDs
    /// @return Reference to this mesh
    Mesh & set_cell_set(Marker id, const std::vector<Index> & cell_ids);

    /// Remove cell sets
    ///
    /// @return Reference to this mesh
    Mesh & remove_cell_sets();

    /// Set side set name
    ///
    /// @param side_set_id Side set ID (marker)
    /// @param name Side set name
    /// @return Reference to this mesh
    Mesh & set_side_set_name(Marker side_set_id, const std::string & name);

    /// Get side set name
    ///
    /// @param side_set_id Side set ID (marker)
    /// @return Side set name
    Optional<std::string> side_set_name(Marker side_set_id) const;

    /// Get side set IDs
    [[nodiscard]] std::vector<Marker> side_set_ids() const;

    /// Get side set
    ///
    /// @param id Side set ID
    /// @return Side set
    [[nodiscard]] Span<const SideEntry> side_set(Marker id) const;

    /// Set side set
    ///
    /// @param id Side set ID
    /// @param side_set Side set
    /// @return Reference to this mesh
    Mesh & set_side_set(Marker id, const std::vector<SideEntry> & side_set);

    /// Remove side sets
    ///
    /// @return Reference to this mesh
    Mesh & remove_side_sets();

    /// Set node set name
    ///
    /// @param id Node set ID
    /// @param name Node set name
    /// @return Reference to this mesh
    Mesh & set_node_set_name(Marker id, const std::string & name);

    /// Get node set name
    ///
    /// @param id Node set ID
    /// @return Node set name
    Optional<std::string> node_set_name(Marker id) const;

    /// Get node set IDs
    ///
    /// @return Node set IDs
    [[nodiscard]] std::vector<Marker> node_set_ids() const;

    /// Get node set
    ///
    /// @param id Node set ID
    /// @return Node set
    [[nodiscard]] Span<const Index> node_set(Marker id) const;

    /// Set node set
    ///
    /// @param id Node set ID
    /// @param node_ids Node IDs
    /// @return Reference to this mesh
    Mesh & set_node_set(Marker id, const std::vector<Index> & node_ids);

    /// Remove node sets
    ///
    /// @return Reference to this mesh
    Mesh & remove_node_sets();

    /// Remap block IDs
    ///
    /// @param block_map Map of old block IDs to new block IDs
    /// @return Reference to this mesh
    Mesh & remap_block_ids(const std::map<Marker, Marker> & block_map);

    /// Get support of a mesh node
    ///
    /// @param index Index of the node
    [[nodiscard]] Span<const Index> support(Index index) const;

    /// Get connectivity of a mesh node
    ///
    /// @param index Index of the node
    [[nodiscard]] Span<const Index> cone(Index index) const;

    /// Get cone vertices of a mesh node
    ///
    /// @param index Index of the node
    /// @return Cone vertices
    [[nodiscard]] std::set<Index> cone_vertices(Index index) const;

    /// Get element type
    ///
    /// @param index Index of the element
    /// @return Element type
    [[nodiscard]] ElementType element_type(Index index) const;

    /// Prepare mesh
    void set_up();

    /// Get all boundary edge IDs
    ///
    /// @return Boundary edge IDs
    [[nodiscard]] std::vector<Index> boundary_edges() const;

    /// Get all boundary face IDs
    ///
    /// @return Boundary face IDs
    [[nodiscard]] std::vector<Index> boundary_faces() const;

    /// Compute centroid
    ///
    /// @param index Index of the element
    /// @return Centroid
    [[nodiscard]] Point compute_centroid(Index index) const;

    /// Compute outward normal
    ///
    /// @param index Index of the edge/face
    /// @return Outward normal
    [[nodiscard]] Vector outward_normal(Index index) const;

private:
    /// Mesh points
    std::vector<Point> pnts_;
    /// All mesh elements. Point, edge, face, and cell IDs are indexing into this container.
    std::vector<Element> elems_;
    /// Cell set names
    std::map<Marker, std::string> cell_set_names_;
    /// Cell sets
    std::map<Marker, std::vector<Index>> cell_sets_;
    /// Side set names
    std::map<Marker, std::string> side_set_names_;
    /// Side sets
    std::map<Marker, std::vector<SideEntry>> side_sets_;
    /// Node set names
    std::map<Marker, std::string> node_set_names_;
    /// Node sets
    std::map<Marker, std::vector<Index>> node_sets_;
    ///
    HasseDiagram hasse_;
};

/// Compute bounding box around the mesh
///
/// @return Bounding box
BoundingBox3D compute_bounding_box(const Mesh & mesh);
BoundingBox3D compute_bounding_box(Ptr<const Mesh> mesh);

/// Options for building mesh from GeomModel
struct MeshBuildOptions {
    enum class ElementSelection {
        TopLevelOnly,
        AllMeshed,
        MaxDimension
    };

    ElementSelection element_selection = ElementSelection::TopLevelOnly;
    Optional<int> spatial_dimension = std::nullopt;
};

/// Build mesh from geometric model
///
/// @param model
/// @param options
/// @return Mesh
Ptr<Mesh> build_mesh(const GeomModel & model, const MeshBuildOptions & options = {});

} // namespace krado
