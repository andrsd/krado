// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/geom_shape.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/mesh.h"
#include "krado/scheme.h"
#include "krado/bounding_box_3d.h"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include <Standard_TypeDef.hxx>
#include <map>

namespace krado {

class MeshVertex;
class MeshCurve;
class MeshSurface;
class MeshVolume;

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
    [[nodiscard]] Ptr<MeshVertex> vertex(int id);

    /// Get model vertices
    ///
    /// @return Vertices
    [[nodiscard]] const std::map<int, Ptr<MeshVertex>> & vertices() const;

    /// Curve
    [[nodiscard]] Ptr<MeshCurve> curve(int id);

    /// Get model curves
    ///
    /// @return Curves
    [[nodiscard]] const std::map<int, Ptr<MeshCurve>> & curves() const;

    /// Surface
    [[nodiscard]] Ptr<MeshSurface> surface(int id);

    /// Get model surfaces
    ///
    /// @return Surfaces
    [[nodiscard]] const std::map<int, Ptr<MeshSurface>> & surfaces() const;

    /// Volume
    [[nodiscard]] Ptr<MeshVolume> volume(int id);

    /// Get model volume
    ///
    /// @return Volume
    [[nodiscard]] const std::map<int, Ptr<MeshVolume>> & volumes() const;

    /// Create vertex mesh
    void mesh_vertex(int id);
    void mesh_vertex(Ptr<MeshVertex> vertex);

    /// Create curve mesh
    void mesh_curve(int id);
    void mesh_curve(Ptr<MeshCurve> curve);

    /// Create surface mesh
    void mesh_surface(int id);
    void mesh_surface(Ptr<MeshSurface> surface);

    /// Create volume mesh
    void mesh_volume(int id);
    void mesh_volume(Ptr<MeshVolume> volume);

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
        return dynamic_cast<T &>(entity->scheme());
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

    GeomShape root_shape_;

    std::map<int, GeomVertex> vtxs_;
    std::map<int, GeomCurve> crvs_;
    std::map<int, GeomSurface> srfs_;
    std::map<int, GeomVolume> vols_;

    TopTools_DataMapOfShapeInteger vtx_id_;
    TopTools_DataMapOfShapeInteger crv_id_;
    TopTools_DataMapOfShapeInteger srf_id_;
    TopTools_DataMapOfShapeInteger vol_id_;

    std::map<int, Ptr<MeshVertex>> mvtxs_;
    std::map<int, Ptr<MeshCurve>> mcrvs_;
    std::map<int, Ptr<MeshSurface>> msurfs_;
    std::map<int, Ptr<MeshVolume>> mvols_;
    /// Mesh points
    std::vector<Point> pnts_;
    /// Mesh elements
    std::vector<Element> elems_;
};

} // namespace krado
