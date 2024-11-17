// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <string>
#include <ostream>

namespace krado {

class Element {
public:
    enum Type { LINE2, TRI3, TETRA4 };

    Element(Type type, const std::vector<int> & vtx_ids, int marker = 0);

    /// Get element type
    ///
    /// @return Element type
    Type type() const;

    /// Get the element marker
    ///
    /// @return The element marker
    int marker() const;

    /// Get number of vertices
    ///
    /// @return Number of vertices
    int num_vertices() const;

    /// Get vertex ID fom local index
    ///
    /// @param idx Local vertex index
    /// @return Vertex ID
    int vertex_id(int idx) const;

    /// Call operator to access vertex index
    ///
    /// @param idx Vertex index
    /// @return Vertex ID
    int operator()(int idx) const;

    /// Get vertex IDs
    ///
    /// @return Vertex IDs
    const std::vector<int> & ids() const;

    /// Set element connectivity. This is good for element renumbering. This cannot be used for
    /// changing element type.
    ///
    /// @param ids Vertex IDs
    void set_ids(const std::vector<int> & ids);

private:
    Type elem_type;
    std::vector<int> vtx_id;
    int mrkr;

public:
    static std::string type(Type type);
    static Element Line2(const std::array<int, 2> & ids, int marker = 0);
    static Element Tri3(const std::array<int, 3> & ids, int marker = 0);
    static Element Tetra4(const std::array<int, 4> & ids, int marker = 0);
};

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Element & el)
{
    stream << "(" << krado::Element::type(el.type()) << ",";
    for (auto & id : el.ids())
        stream << " " << id;
    stream << ")";
    return stream;
}
