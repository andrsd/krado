// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// original file: meshGFaceBDS.h

#pragma once

namespace krado {

class MeshSurface;
class BDS_Mesh;

void delaunayize_bds(MeshSurface & msurface, BDS_Mesh & m, int & nb_swap);

} // namespace krado
