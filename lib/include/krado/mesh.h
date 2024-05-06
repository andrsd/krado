#pragma once

#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_point.h"
#include <map>

namespace krado {

class Model;

class Mesh {
public:
    Mesh(const Model & model);

    /// Vertex
    const MeshVertex & vertex(int id) const;

    /// Vertices
    const std::map<int, MeshVertex> & vertices() const;

    /// Curve
    const MeshCurve & curve(int id) const;

    /// Curves
    const std::map<int, MeshCurve> & curves() const;

    /// Surface
    const MeshSurface & surface(int id) const;

    /// Surfaces
    const std::map<int, MeshSurface> & surfaces() const;

    /// Volume
    const MeshVolume & volume(int id) const;

    /// Volumes
    const std::map<int, MeshVolume> & volumes() const;

private:
    void initialize(const Model & model);

    std::map<int, MeshVertex> vtxs;
    std::map<int, MeshCurve> crvs;
    std::map<int, MeshSurface> surfs;
    std::map<int, MeshVolume> vols;
};

} // namespace krado
