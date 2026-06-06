// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/types.h"
#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/scheme1d.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/uv_param.h"
#include <map>

namespace krado {

class MeshSurface;
class MeshCurve;

class SchemeDelaunay : public Scheme, public Scheme2D {
public:
    SchemeDelaunay(const std::string & name);
    void mesh_surface(Ptr<MeshSurface> surface) override;

protected:
    bool
    mesh_generation(Ptr<MeshSurface> surface, Span<Ptr<MeshCurve>> curves, bool only_initial_mesh);

private:
    virtual void insertion_algo(Ptr<MeshSurface> surface) = 0;

public:
    static const std::string name;
};

///
void bowyer_watson(Ptr<MeshSurface> surface, int MAXPNT);

///
void bowyer_watson_frontal(
    Ptr<MeshSurface> surface,
    std::map<Ptr<MeshVertexAbstract>, Ptr<MeshVertexAbstract>> * equivalence = nullptr,
    std::map<Ptr<MeshVertexAbstract>, UVParam> * parametricCoordinates = nullptr,
    std::vector<UVParam> * true_boundary = nullptr);

} // namespace krado
