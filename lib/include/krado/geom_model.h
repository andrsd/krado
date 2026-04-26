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
#include "krado/bounding_box_3d.h"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include <map>

class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Face;
class TopoDS_Shell;
class TopoDS_Solid;

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
    [[nodiscard]] const GeomVertex & geom_vertex(ShapeID id) const;
    [[nodiscard]] GeomVertex & geom_vertex(ShapeID id);

    /// Get curve with specified ID
    ///
    /// @param id Curve id
    /// @return Curve with specified ID
    [[nodiscard]] const GeomCurve & geom_curve(ShapeID id) const;
    [[nodiscard]] GeomCurve & geom_curve(ShapeID id);

    /// Get surface with specified ID
    ///
    /// @param id Surface ID
    /// @return Surface with specified ID
    [[nodiscard]] const GeomSurface & geom_surface(ShapeID id) const;
    [[nodiscard]] GeomSurface & geom_surface(ShapeID id);

    /// Get volume with specified ID
    ///
    /// @param id Volume ID
    /// @return Volume with specified ID
    [[nodiscard]] const GeomVolume & geom_volume(ShapeID id) const;
    [[nodiscard]] GeomVolume & geom_volume(ShapeID id);

    /// Vertex
    [[nodiscard]] Ptr<MeshVertex> vertex(ShapeID id);

    /// Get model vertices
    ///
    /// @return Vertices
    [[nodiscard]] const std::map<ShapeID, Ptr<MeshVertex>> & vertices() const;

    /// Curve
    [[nodiscard]] Ptr<MeshCurve> curve(ShapeID id);

    /// Get model curves
    ///
    /// @return Curves
    [[nodiscard]] const std::map<ShapeID, Ptr<MeshCurve>> & curves() const;

    /// Surface
    [[nodiscard]] Ptr<MeshSurface> surface(ShapeID id);

    /// Get model surfaces
    ///
    /// @return Surfaces
    [[nodiscard]] const std::map<ShapeID, Ptr<MeshSurface>> & surfaces() const;

    /// Volume
    [[nodiscard]] Ptr<MeshVolume> volume(ShapeID id);

    /// Get model volume
    ///
    /// @return Volume
    [[nodiscard]] const std::map<ShapeID, Ptr<MeshVolume>> & volumes() const;

    /// Create vertex mesh
    void mesh_vertex(ShapeID id);
    void mesh_vertex(Ptr<MeshVertex> vertex);

    /// Create curve mesh
    void mesh_curve(ShapeID id);
    void mesh_curve(Ptr<MeshCurve> curve);

    /// Create surface mesh
    void mesh_surface(ShapeID id);
    void mesh_surface(Ptr<MeshSurface> surface);

    /// Create volume mesh
    void mesh_volume(ShapeID id);
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
    [[nodiscard]] ShapeID vertex_id(const GeomVertex & vertex) const;

    /// Get curve ID
    ///
    /// @param curve Curve
    /// @return Curve ID
    [[nodiscard]] ShapeID curve_id(const GeomCurve & curve) const;

    /// Get surface ID
    ///
    /// @param surface Surface
    /// @return Surface ID
    [[nodiscard]] ShapeID surface_id(const GeomSurface & surface) const;

    /// Get volume ID
    ///
    /// @param volume Volume
    /// @return Volume ID
    [[nodiscard]] ShapeID volume_id(const GeomVolume & volume) const;

private:
    ShapeID get_shape_id(const TopoDS_Vertex & vertex);
    ShapeID get_shape_id(const TopoDS_Edge & edge);
    ShapeID get_shape_id(const TopoDS_Face & face);
    ShapeID get_shape_id(const TopoDS_Shell & shell);
    ShapeID get_shape_id(const TopoDS_Solid & solid);

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

    std::map<ShapeID, GeomVertex> vtxs_;
    std::map<ShapeID, GeomCurve> crvs_;
    std::map<ShapeID, GeomSurface> srfs_;
    std::map<ShapeID, GeomVolume> vols_;

    TopTools_DataMapOfShapeInteger vtx_id_;
    TopTools_DataMapOfShapeInteger crv_id_;
    TopTools_DataMapOfShapeInteger srf_id_;
    TopTools_DataMapOfShapeInteger vol_id_;

    std::map<ShapeID, Ptr<MeshVertex>> mvtxs_;
    std::map<ShapeID, Ptr<MeshCurve>> mcrvs_;
    std::map<ShapeID, Ptr<MeshSurface>> msurfs_;
    std::map<ShapeID, Ptr<MeshVolume>> mvols_;
};

} // namespace krado
