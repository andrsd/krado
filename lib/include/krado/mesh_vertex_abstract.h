#pragma once

#include "krado/point.h"

namespace krado {

class MeshVertexAbstract {
public:
    MeshVertexAbstract();

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    virtual Point point() const = 0;

    /// Get global ID
    ///
    /// @return Global ID of this vertex
    int global_id() const;

    /// Set global ID
    ///
    /// @param id New ID to assign
    void set_global_id(int id);

private:
    int gid;
};

} // namespace krado
