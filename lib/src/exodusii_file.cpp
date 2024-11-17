#include "krado/exodusii_file.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "exodusIIcpp/exception.h"
#include "krado/utils.h"

namespace krado {

Element::Type
element_type(const std::string elem_type_name)
{
    auto ellc = utils::to_lower(elem_type_name);
    if (utils::in(ellc, { "bar", "bar2" }))
        return Element::LINE2;
    else if (utils::in(ellc, { "tri3", "tri" }))
        return Element::TRI3;
    else if (utils::in(ellc, { "tet", "tet4", "tetra", "tetra4" }))
        return Element::TETRA4;
    else
        throw std::runtime_error("Unsupported element type: " + elem_type_name);
}

template <int N>
std::array<int, N>
build_element_connect(const std::vector<int> & connect, int idx)
{
    std::array<int, N> elem_connect;
    for (int i = 0; i < N; i++)
        elem_connect[i] = connect[idx + i] - 1;
    return elem_connect;
}

ExodusIIFile::ExodusIIFile(const std::string & file_name) : fn(file_name) {}

Mesh
ExodusIIFile::read()
{
    this->exo.open(this->fn);
    this->exo.init();
    auto pnts = read_points();
    auto elems = read_elements();
    this->exo.close();
    return { pnts, elems };
}

std::vector<Point>
ExodusIIFile::read_points()
{
    std::vector<Point> points;

    this->exo.read_coords();
    int dim = this->exo.get_dim();
    int n_nodes = this->exo.get_num_nodes();
    if (dim == 1) {
        auto x = this->exo.get_x_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i]);
    }
    else if (dim == 2) {
        auto x = this->exo.get_x_coords();
        auto y = this->exo.get_y_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i], y[i]);
    }
    else if (dim == 3) {
        auto x = this->exo.get_x_coords();
        auto y = this->exo.get_y_coords();
        auto z = this->exo.get_z_coords();
        for (auto i = 0; i < n_nodes; i++)
            points.emplace_back(x[i], y[i], z[i]);
    }
    return points;
}

std::vector<Element>
ExodusIIFile::read_elements()
{
    std::vector<Element> elems;

    this->exo.read_blocks();
    for (auto & eb : this->exo.get_element_blocks()) {
        auto et = element_type(eb.get_element_type());
        auto connect = eb.get_connectivity();
        auto n_elem_nodes = eb.get_num_nodes_per_element();
        auto blk_id = eb.get_id();
        for (int i = 0; i < eb.get_num_elements(); i++) {
            auto idx = i * n_elem_nodes;
            if (et == Element::LINE2) {
                auto elem_connect = build_element_connect<2>(connect, idx);
                auto el = Element::Line2(elem_connect, blk_id);
                elems.emplace_back(el);
            }
            else if (et == Element::TRI3) {
                auto elem_connect = build_element_connect<3>(connect, idx);
                auto el = Element::Tri3(elem_connect, blk_id);
                elems.emplace_back(el);
            }
            else if (et == Element::TETRA4) {
                auto elem_connect = build_element_connect<4>(connect, idx);
                auto el = Element::Tetra4(elem_connect, blk_id);
                elems.emplace_back(el);
            }
            else
                throw std::runtime_error("Unsupported element type: " + eb.get_element_type());
        }
    }

    return elems;
}

} // namespace krado
