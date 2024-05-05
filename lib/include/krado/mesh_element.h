#pragma once

#include <vector>

namespace krado {

class MeshElement {
public:
    enum Type { LINE2, TRI3, TETRA4 };

    MeshElement(Type type, const std::vector<int> vtx_ids);

    /// Get element type
    ///
    /// @return Element type
    Type type() const;

    /// Get number of vertices
    ///
    /// @return Number of vertices
    int num_vertices() const;

private:
    Type elem_type;
    std::vector<int> vertex_id;

public:
    static MeshElement Line2(int v1, int v2);
    static MeshElement Tri3(int v1, int v2, int v3);
    static MeshElement Tetra4(int v1, int v2, int v3, int v4);
};

} // namespace krado
