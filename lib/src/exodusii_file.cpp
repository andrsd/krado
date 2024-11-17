#include "krado/exodusii_file.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "krado/config.h"
#include "krado/utils.h"
#include "exodusIIcpp/exception.h"
#include "fmt/format.h"
#include "fmt/chrono.h"

namespace krado {

namespace {

const char *
get_exodusii_name(Element::Type t)
{
    switch (t) {
    case Element::LINE2:
        return "BAR2";
    case Element::TRI3:
        return "TRI3";
    case Element::TETRA4:
        return "TET4";
    }
    throw Exception("Unsupported element type {}.", t);
}

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

} // namespace

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

void
ExodusIIFile::write(const Mesh & mesh)
{
    this->exo.create(this->fn);
    this->dim = determine_spatial_dim(mesh);
    preprocess_mesh(mesh);

    int n_nodes = (int) mesh.points().size();
    int n_elems = (int) mesh.elements().size();
    int n_elem_blks = this->elem_blks.size();
    int n_node_sets = 0, n_side_sets = 0;
    this->exo.init("", this->dim, n_nodes, n_elems, n_elem_blks, n_node_sets, n_side_sets);

    write_info();
    write_coords();
    write_elements();

    this->exo.close();
}

int
ExodusIIFile::determine_spatial_dim(const Mesh & mesh)
{
    auto sz = mesh.bounding_box().size();
    if ((sz[0] > 0) && (sz[1] < 1e-15) && (sz[2] < 1e-15))
        return 1;
    else if ((sz[0] > 0) && (sz[1] > 0) && (sz[2] < 1e-15))
        return 2;
    else if ((sz[0] > 0) && (sz[1] > 0) && (sz[2] > 0))
        return 3;
    else
        throw Exception("Unusual mesh, unable to write.");
}

void
ExodusIIFile::write_info()
{
    std::time_t now = std::time(nullptr);
    std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", fmt::localtime(now));

    std::vector<std::string> info(1);
    info[0] = fmt::format("Created by krado v{} on {}", KRADO_VERSION, datetime);
    this->exo.write_info(info);
}

void
ExodusIIFile::write_coords()
{
    if (this->dim == 1)
        this->exo.write_coords(this->x);
    else if (this->dim == 2)
        this->exo.write_coords(this->x, this->y);
    else if (this->dim == 3)
        this->exo.write_coords(this->x, this->y, this->z);
    this->exo.write_coord_names();
}

void
ExodusIIFile::write_elements()
{
    for (auto & [blk_id, elems] : this->elem_blks) {
        if (!elems.empty()) {
            auto el_type = get_exodusii_name(elems[0].type());
            auto n = elems[0].num_vertices() * elems.size();
            std::vector<int> connect;
            connect.reserve(n);
            for (auto & el : elems) {
                for (int j = 0; j < el.ids().size(); j++)
                    connect.push_back(el.vertex_id(j) + 1);
            }
            this->exo.write_block(blk_id, el_type, elems.size(), connect);
        }
    }
}

void
ExodusIIFile::preprocess_mesh(const Mesh & mesh)
{
    for (auto & pt : mesh.points()) {
        if (this->dim >= 1)
            this->x.push_back(pt.x);
        if (this->dim >= 2)
            this->y.push_back(pt.y);
        if (this->dim >= 3)
            this->z.push_back(pt.z);
    }

    for (auto & el : mesh.elements()) {
        auto marker = el.marker();
        this->elem_blks[marker].push_back(el);
    }
}

} // namespace krado
