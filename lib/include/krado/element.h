// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <string>
#include <ostream>

namespace krado {

using marker_t = int;

class Element {
public:
    enum Type { LINE2, TRI3, TETRA4 };

    /// Build an element
    ///
    /// @param type Element type
    /// @param vtx_ids Vertex IDs composing the element
    /// @param marker Element marker
    Element(Type type, const std::vector<int> & vtx_ids, marker_t marker = 0);

    /// Get element type
    ///
    /// @return Element type
    Type type() const;

    /// Get the element marker
    ///
    /// @return The element marker
    marker_t marker() const;

    /// Set the element marker
    ///
    /// @param marker The element marker
    void set_marker(marker_t marker);

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
    marker_t mrkr;

public:
    static std::string type(Type type);
    static Element Line2(const std::array<int, 2> & ids, marker_t marker = 0);
    static Element Tri3(const std::array<int, 3> & ids, marker_t marker = 0);
    static Element Tetra4(const std::array<int, 4> & ids, marker_t marker = 0);
};

bool operator==(const Element & a, const Element & b);

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
