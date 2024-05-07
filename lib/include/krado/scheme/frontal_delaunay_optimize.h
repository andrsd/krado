// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// Orignal file: meshGFaceOptimize.h

#pragma once

#include "krado/scheme/frontal_delaunay_insertion.h"
#include <map>
#include <vector>

namespace krado {

class MeshSurface;

bool build_mesh_generation_data_structures(MeshSurface & surface,
                                           std::set<MTri3 *, MTri3::ComparePtr> & all_tris,
                                           BidimMeshData & data);
void transfer_data_structure(MeshSurface & gf,
                             std::set<MTri3 *, MTri3::ComparePtr> & all_tris,
                             BidimMeshData & data);
void compute_equivalences(MeshSurface & gf, BidimMeshData & data);

struct SwapQuad {
    int v[4];
    bool
    operator<(const SwapQuad & o) const
    {
        if (v[0] < o.v[0])
            return true;
        if (v[0] > o.v[0])
            return false;
        if (v[1] < o.v[1])
            return true;
        if (v[1] > o.v[1])
            return false;
        if (v[2] < o.v[2])
            return true;
        if (v[2] > o.v[2])
            return false;
        if (v[3] < o.v[3])
            return true;
        return false;
    }

    SwapQuad(MeshVertexAbstract * v1,
             MeshVertexAbstract * v2,
             MeshVertexAbstract * v3,
             MeshVertexAbstract * v4)
    {
        v[0] = v1->num();
        v[1] = v2->num();
        v[2] = v3->num();
        v[3] = v4->num();
        std::sort(v, v + 4);
    }

    SwapQuad(int const v1, int const v2, int const v3, int const v4)
    {
        v[0] = v1;
        v[1] = v2;
        v[2] = v3;
        v[3] = v4;
        std::sort(v, v + 4);
    }
};

} // namespace krado
