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
    MeshSurface(ShapeID id, const GeomSurface & gcurve, std::vector<Ptr<MeshCurve>> mesh_curves);
    ~MeshSurface();

    /// Get the unique identifier of the surface.
    ///
    /// @return The unique identifier of the surface.
    ShapeID id() const;

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

    /// Get mesh size at given surface parameter
    ///
    /// @param par Surface parameter (u, v)
    /// @return Mesh size at the parameter
    [[nodiscard]] double mesh_size_at_param(UVParam par) const;

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

    Span<const MeshElement> elements() const;

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

    bool has_scheme() const;

    Scheme2D & scheme();

private:
    ///
    ShapeID id_;
    ///
    const GeomSurface & gsurface_;
    /// Mesh curves bounding this surface
    std::vector<Ptr<MeshCurve>> mesh_curves_;
    /// Surface vertices (not including boundary and mesh vertices)
    std::vector<Ptr<MeshSurfaceVertex>> surf_vtxs_;
    /// Triangles
    std::vector<MeshElement> tris_;
    /// Quadrangles
    std::vector<MeshElement> quads_;
    /// Mesh size for the surface
    Optional<double> mesh_size_;
    ///
    std::unique_ptr<Scheme2D> scheme_;
};

} // namespace krado

std::ostream & operator<<(std::ostream & stream, const krado::MeshSurface & srf);
