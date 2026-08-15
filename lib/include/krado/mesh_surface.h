// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_element.h"
#include "krado/meshable.h"
#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/ptr.h"
#include <vector>
#include <memory>

namespace krado {

class UVParam;
class GeomSurface;
class MeshVertex;
class MeshVertexAbstract;
class MeshCurveVertex;
class MeshSurfaceVertex;
class MeshCurve;

class MeshSurface : public Meshable {
public:
    MeshSurface(ShapeID id, GeomSurface & gcurve, std::vector<Ptr<MeshCurve>> mesh_curves);
    ~MeshSurface();

    /// Get the unique identifier of the surface.
    ///
    /// @return The unique identifier of the surface.
    [[nodiscard]] ShapeID id() const;

    /// Get geometrical surface associated with this surface
    ///
    /// @return Geometrical surface associated with this surface
    [[nodiscard]] const GeomSurface & geom_surface() const;

    /// Get curves bounding this surface
    [[nodiscard]] Span<const Ptr<MeshCurve>> curves() const;
    [[nodiscard]] Span<Ptr<MeshCurve>> curves();

    /// Get the mesh size for this surface
    ///
    /// @return The mesh size
    [[nodiscard]] double mesh_size() const;

    /// Set the mesh size for this surface
    ///
    /// @param size The new mesh size
    void set_mesh_size(double size);

    /// Get (internal) vertices on the surface
    ///
    /// @return Vertices on the surface
    [[nodiscard]] Span<const Ptr<MeshSurfaceVertex>> surface_vertices() const;

    [[nodiscard]] Span<Ptr<MeshSurfaceVertex>> surface_vertices();

    /// Get triangles on this surface
    ///
    /// @return Triangles on this surface
    [[nodiscard]] Span<const MeshElement> triangles() const;

    [[nodiscard]] Span<MeshElement> triangles();

    /// Get quadrangles on this surface
    ///
    /// @return Quadrangles on this surface
    [[nodiscard]] Span<const MeshElement> quadrangles() const;

    [[nodiscard]] Span<MeshElement> quadrangles();

    /// Add vertex
    ///
    /// @param vertex Vertex to add
    void add_vertex(Ptr<MeshSurfaceVertex> vertex);

    /// Add new triangle
    ///
    /// @param tri Local vertex indices
    void add_triangle(const std::array<Ptr<MeshVertexAbstract>, 3> & tri);

    /// Add new quadrangle
    ///
    /// @param quad Local vertices
    void add_quadrangle(const std::array<Ptr<MeshVertexAbstract>, 4> & quad);

    void add_element(MeshElement tri);

    /// Convert all quadrangles to triangles
    ///
    /// @param mode Splitting mode
    void quads_to_tris(QuadSplitMode mode = QuadSplitMode::SPLIT2);

    /// Reserve memory for vertices and triangles
    void reserve_mem(std::size_t n_tris);

    void set_triangles(const std::vector<MeshElement> & new_tris);

    [[nodiscard]] Span<const MeshElement> elements() const;

    void remove_all_triangles();

    void delete_mesh();

    /// Set meshing scheme
    ///
    /// @param name Name od the scheme to assign
    /// @return Pointer to the scheme
    template <typename SCHEME>
        requires std::derived_from<SCHEME, Scheme>
    SCHEME &
    set_scheme(SCHEME::Options options)
    {
        auto sch = std::make_unique<SCHEME>(options);
        auto sch_ptr = sch.get();
        this->scheme_ = std::move(sch);
        return *sch_ptr;
    }

    [[nodiscard]] bool has_scheme() const;

    Scheme2D & scheme();

private:
    ///
    ShapeID id_;
    ///
    GeomSurface & gsurface_;
    /// Mesh curves bounding this surface
    std::vector<Ptr<MeshCurve>> mesh_curves_;
    /// Surface vertices (not including boundary and mesh vertices)
    std::vector<Ptr<MeshSurfaceVertex>> surf_vtxs_;
    /// Triangles
    std::vector<MeshElement> tris_;
    /// Quadrangles
    std::vector<MeshElement> quads_;
    ///
    std::unique_ptr<Scheme2D> scheme_;
};

/// Create a counter-clock-wise triangle
///
/// @param gsurf Geomterical surface
/// @param a Vertex A
/// @param b Vertex B
/// @param c Vertex C
/// @return Counter-clockwise triangle
///
/// NOTE: this should end up in some kind of triangularization module
std::array<Ptr<MeshVertexAbstract>, 3> ccw_triangle(const GeomSurface & gsurf,
                                                    Ptr<MeshVertexAbstract> a,
                                                    Ptr<MeshVertexAbstract> b,
                                                    Ptr<MeshVertexAbstract> c);

/// Create a counter-clock-wise quadrangle
///
/// @param gsurf Geomterical surface
/// @param a Vertex A
/// @param b Vertex B
/// @param c Vertex C
/// @param d Vertex D
/// @return Counter-clockwise quadrangle
std::array<Ptr<MeshVertexAbstract>, 4> ccw_quadrangle(const GeomSurface & gsurf,
                                                      Ptr<MeshVertexAbstract> a,
                                                      Ptr<MeshVertexAbstract> b,
                                                      Ptr<MeshVertexAbstract> c,
                                                      Ptr<MeshVertexAbstract> d);

} // namespace krado

std::ostream & operator<<(std::ostream & stream, const krado::MeshSurface & srf);
