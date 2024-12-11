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
#include <map>

namespace krado {

class GeomModel {
public:
    explicit GeomModel(const GeomShape & root_shape);

    /// Get vertex with specified ID
    ///
    /// @param id Vertex id
    /// @return Vertex with specified ID
    const GeomVertex & geom_vertex(int id) const;
    GeomVertex & geom_vertex(int id);

    /// Get curve with specified ID
    ///
    /// @param id Curve id
    /// @return Curve with specified ID
    const GeomCurve & geom_curve(int id) const;
    GeomCurve & geom_curve(int id);

    /// Get surface with specified ID
    ///
    /// @param id Surface ID
    /// @return Surface with specified ID
    const GeomSurface & geom_surface(int id) const;
    GeomSurface & geom_surface(int id);

    /// Get volume with specified ID
    ///
    /// @param id Volume ID
    /// @return Volume with specified ID
    const GeomVolume & geom_volume(int id) const;
    GeomVolume & geom_volume(int id);

    /// Vertex
    const MeshVertex & vertex(int id) const;
    MeshVertex & vertex(int id);

    /// Get model vertices
    ///
    /// @return Vertices
    const std::map<int, MeshVertex> & vertices() const;

    /// Curve
    const MeshCurve & curve(int id) const;
    MeshCurve & curve(int id);

    /// Get model curves
    ///
    /// @return Curves
    const std::map<int, MeshCurve> & curves() const;

    /// Surface
    const MeshSurface & surface(int id) const;
    MeshSurface & surface(int id);

    /// Get model surfaces
    ///
    /// @return Surfaces
    const std::map<int, MeshSurface> & surfaces() const;

    /// Volume
    const MeshVolume & volume(int id) const;
    MeshVolume & volume(int id);

    /// Get model volume
    ///
    /// @return Volume
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

    /// Build the mesh from meshed entities
    Mesh build_mesh();

protected:
    /// Get vertex ID
    ///
    /// @param vertex Vertex
    /// @return Vertex ID
    int vertex_id(const GeomVertex & vertex) const;

    /// Get curve ID
    ///
    /// @param curve Curve
    /// @return Curve ID
    int curve_id(const GeomCurve & curve) const;

    /// Get surface ID
    ///
    /// @param surface Surface
    /// @return Surface ID
    int surface_id(const GeomSurface & surface) const;

    /// Get volume ID
    ///
    /// @param volume Volume
    /// @return Volume ID
    int volume_id(const GeomVolume & volume) const;

    template <typename T, typename U>
    T &
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

    BoundingBox3D compute_mesh_bounding_box();
    std::vector<Point> build_points();
    std::vector<Element> build_elements();
    std::vector<Element> build_1d_elements();
    std::vector<Element> build_2d_elements();

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
