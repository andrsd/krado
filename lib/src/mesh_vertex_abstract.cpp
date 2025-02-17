// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex_abstract.h"

namespace krado {

MeshVertexAbstract::MeshVertexAbstract() : gid_(0) {}

int
MeshVertexAbstract::global_id() const
{
    return this->gid_;
}

void
MeshVertexAbstract::set_global_id(int id)
{
    this->gid_ = id;
}

} // namespace krado
