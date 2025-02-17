// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/mesh.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/scheme.h"
#include "krado/bounding_box_3d.h"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include <Standard_TypeDef.hxx>
#include <map>

namespace krado {

class GeomModel {
public:
    explicit GeomModel(const GeomShape & root_shape);

    /// Get vertex with specified ID
    ///
    /// @param id Vertex id
    /// @return Vertex with specified ID
    [[nodiscard]] const GeomVertex & geom_vertex(int id) const;
    [[nodiscard]] GeomVertex & geom_vertex(int id);

    /// Get curve with specified ID
    ///
    /// @param id Curve id
    /// @return Curve with specified ID
    [[nodiscard]] const GeomCurve & geom_curve(int id) const;
    [[nodiscard]] GeomCurve & geom_curve(int id);

    /// Get surface with specified ID
    ///
    /// @param id Surface ID
    /// @return Surface with specified ID
    [[nodiscard]] const GeomSurface & geom_surface(int id) const;
    [[nodiscard]] GeomSurface & geom_surface(int id);

    /// Get volume with specified ID
    ///
    /// @param id Volume ID
    /// @return Volume with specified ID
    [[nodiscard]] const GeomVolume & geom_volume(int id) const;
    [[nodiscard]] GeomVolume & geom_volume(int id);

    /// Vertex
    [[nodiscard]] const MeshVertex & vertex(int id) const;
    [[nodiscard]] MeshVertex & vertex(int id);

    /// Get model vertices
    ///
    /// @return Vertices
    [[nodiscard]] const std::map<int, MeshVertex> & vertices() const;

    /// Curve
    [[nodiscard]] const MeshCurve & curve(int id) const;
    [[nodiscard]] MeshCurve & curve(int id);

    /// Get model curves
    ///
    /// @return Curves
    [[nodiscard]] const std::map<int, MeshCurve> & curves() const;

    /// Surface
    [[nodiscard]] const MeshSurface & surface(int id) const;
    [[nodiscard]] MeshSurface & surface(int id);

    /// Get model surfaces
    ///
    /// @return Surfaces
    [[nodiscard]] const std::map<int, MeshSurface> & surfaces() const;

    /// Volume
    [[nodiscard]] const MeshVolume & volume(int id) const;
    [[nodiscard]] MeshVolume & volume(int id);

    /// Get model volume
    ///
    /// @return Volume
    [[nodiscard]] const std::map<int, MeshVolume> & volumes() const;

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

    /// Build the mesh from meshed entities
    [[nodiscard]] Mesh build_mesh();

    /// Build the surface mesh from meshed entities
    [[nodiscard]] Mesh build_surface_mesh();

protected:
    /// Get vertex ID
    ///
    /// @param vertex Vertex
    /// @return Vertex ID
    [[nodiscard]] int vertex_id(const GeomVertex & vertex) const;

    /// Get curve ID
    ///
    /// @param curve Curve
    /// @return Curve ID
    [[nodiscard]] int curve_id(const GeomCurve & curve) const;

    /// Get surface ID
    ///
    /// @param surface Surface
    /// @return Surface ID
    [[nodiscard]] int surface_id(const GeomSurface & surface) const;

    /// Get volume ID
    ///
    /// @param volume Volume
    /// @return Volume ID
    [[nodiscard]] int volume_id(const GeomVolume & volume) const;

    template <typename T, typename U>
    [[nodiscard]] T &
    get_scheme(U entity) const
    {
        return dynamic_cast<T &>(entity.scheme());
    }

private:
    void bind_shape(const GeomShape & shape);
    void bind_vertices(const GeomShape & shape);
    void bind_edges(const GeomShape & shape);
    void bind_faces(const GeomShape & shape);
    void bind_solids(const GeomShape & shape);
    void initialize();

    [[nodiscard]] BoundingBox3D compute_mesh_bounding_box();
    [[nodiscard]] std::vector<Point> build_points();
    [[nodiscard]] std::vector<Element> build_elements();
    [[nodiscard]] std::vector<Element> build_surface_elements();
    [[nodiscard]] std::vector<Element> build_1d_elements();
    [[nodiscard]] std::vector<Element> build_2d_elements();

    GeomShape root_shape;

    std::map<int, GeomVertex> vtxs;
    std::map<int, GeomCurve> crvs;
    std::map<int, GeomSurface> srfs;
    std::map<int, GeomVolume> vols;

    TopTools_DataMapOfShapeInteger vtx_id;
    TopTools_DataMapOfShapeInteger crv_id;
    TopTools_DataMapOfShapeInteger srf_id;
    TopTools_DataMapOfShapeInteger vol_id;

    std::map<int, MeshVertex> mvtxs;
    std::map<int, MeshCurve> mcrvs;
    std::map<int, MeshSurface> msurfs;
    std::map<int, MeshVolume> mvols;
    /// Mesh points
    std::vector<Point> pnts;
    /// Mesh elements
    std::vector<Element> elems;
};

} // namespace krado
