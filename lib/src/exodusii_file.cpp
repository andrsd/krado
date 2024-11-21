#include "krado/exodusii_file.h"
#include "krado/exception.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "krado/config.h"
#include "krado/utils.h"
#include "exodusIIcpp/exception.h"
#include "fmt/format.h"
#include "fmt/chrono.h"

namespace krado {

namespace exII {

const char *
element_name(Element::Type t)
{
    switch (t) {
    case Element::LINE2:
        return "BAR2";
    case Element::TRI3:
        return "TRI3";
    case Element::QUAD4:
        return "QUAD4";
    case Element::TETRA4:
        return "TET4";
    case Element::PYRAMID5:
        return "PYRAMID5";
    case Element::PRISM6:
        return "WEDGE6";
    case Element::HEX8:
        return "HEX8";
    default:
        break;
    }
    throw Exception("Unsupported element type {}.", t);
}

Element::Type
element_type(const std::string elem_type_name)
{
    auto ellc = utils::to_lower(elem_type_name);
    if (utils::in(ellc, { "circle", "sphere" }))
        return Element::POINT;
    else if (utils::in(ellc, { "bar", "bar2" }))
        return Element::LINE2;
    else if (utils::in(ellc, { "tri3", "tri" }))
        return Element::TRI3;
    else if (utils::in(ellc, { "quad", "quad4" }))
        return Element::QUAD4;
    else if (utils::in(ellc, { "tet", "tet4", "tetra", "tetra4" }))
        return Element::TETRA4;
    else if (utils::in(ellc, { "pyramid", "pyramid5", "pyr5" }))
        return Element::PYRAMID5;
    else if (utils::in(ellc, { "wedge", "wedge6" }))
        return Element::PRISM6;
    else if (utils::in(ellc, { "hex", "hex8" }))
        return Element::HEX8;
    else
        throw std::runtime_error("Unsupported element type: " + elem_type_name);
}

template <int N>
std::array<std::size_t, N>
build_element_connect(const std::vector<int> & connect, int idx)
{
    std::array<std::size_t, N> elem_connect;
    for (int i = 0; i < N; i++)
        elem_connect[i] = connect[idx + i] - 1;
    return elem_connect;
}

/// Remap krado side index to exodusII side index
int
local_side_index(Element::Type et, int idx)
{
    if (utils::in(et, { Element::Type::LINE2, Element::Type::TRI3, Element::Type::QUAD4 }))
        return idx + 1;
    else if (et == Element::Type::TETRA4) {
        std::array<int, 4> tetra_sides = { 4, 1, 2, 3 };
        return tetra_sides[idx];
    }
    else {
        throw Exception("Unsupported element type {}", Element::type(et));
    }
}

} // namespace exII

template <typename ET>
Element build_element(const std::vector<int> & connect, int idx, int blk_id);

template <>
Element
build_element<Line2>(const std::vector<int> & connect, int idx, int blk_id)
{
    auto elem_connect = exII::build_element_connect<2>(connect, idx);
    return Element::Line2(elem_connect, blk_id);
}

template <>
Element
build_element<Tri3>(const std::vector<int> & connect, int idx, int blk_id)
{
    auto elem_connect = exII::build_element_connect<3>(connect, idx);
    return Element::Tri3(elem_connect, blk_id);
}

template <>
Element
build_element<Quad4>(const std::vector<int> & connect, int idx, int blk_id)
{
    auto elem_connect = exII::build_element_connect<4>(connect, idx);
    return Element::Quad4(elem_connect, blk_id);
}

template <>
Element
build_element<Tetra4>(const std::vector<int> & connect, int idx, int blk_id)
{
    auto elem_connect = exII::build_element_connect<4>(connect, idx);
    return Element::Tetra4(elem_connect, blk_id);
}

//

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
        auto et = exII::element_type(eb.get_element_type());
        auto connect = eb.get_connectivity();
        auto n_elem_nodes = eb.get_num_nodes_per_element();
        auto blk_id = eb.get_id();
        for (int i = 0; i < eb.get_num_elements(); i++) {
            auto idx = i * n_elem_nodes;
            if (et == Element::LINE2)
                elems.emplace_back(build_element<Line2>(connect, idx, blk_id));
            else if (et == Element::TRI3)
                elems.emplace_back(build_element<Tri3>(connect, idx, blk_id));
            else if (et == Element::QUAD4)
                elems.emplace_back(build_element<Quad4>(connect, idx, blk_id));
            else if (et == Element::TETRA4)
                elems.emplace_back(build_element<Tetra4>(connect, idx, blk_id));
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

    int n_nodes = (int) mesh.points().size();
    int n_elems = (int) mesh.cell_ids().size();
    int n_elem_blks = mesh.cell_set_ids().empty() ? 1 : mesh.cell_set_ids().size();
    int n_node_sets = 0;
    int n_side_sets = mesh.side_set_ids().size();
    this->exo.init("", this->dim, n_nodes, n_elems, n_elem_blks, n_node_sets, n_side_sets);

    write_info();
    write_coords(mesh);
    write_elements(mesh);
    write_side_sets(mesh);

    this->exo.close();
}

int
ExodusIIFile::determine_spatial_dim(const Mesh & mesh)
{
    auto sz = mesh.compute_bounding_box().size();
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
ExodusIIFile::write_coords(const Mesh & mesh)
{
    std::vector<double> x, y, z;
    for (auto & pt : mesh.points()) {
        if (this->dim >= 1)
            x.push_back(pt.x);
        if (this->dim >= 2)
            y.push_back(pt.y);
        if (this->dim >= 3)
            z.push_back(pt.z);
    }

    if (this->dim == 1)
        this->exo.write_coords(x);
    else if (this->dim == 2)
        this->exo.write_coords(x, y);
    else if (this->dim == 3)
        this->exo.write_coords(x, y, z);
    this->exo.write_coord_names();
}

void
ExodusIIFile::write_elements(const Mesh & mesh)
{
    if (mesh.cell_set_ids().empty()) {
        std::map<Element::Type, std::vector<std::size_t>> elem_blks;
        int exii_idx = 1;
        for (auto & cell_id : mesh.cell_ids()) {
            this->exii_elem_ids[cell_id] = exii_idx++;
            auto & cell = mesh.element(cell_id);
            auto et = cell.type();
            elem_blks[et].push_back(cell_id);
        }

        int blk_id = 1;
        for (auto & [blk_type, elems] : elem_blks) {
            if (!elems.empty()) {
                auto cell_id = elems[0];
                auto & cell = mesh.element(cell_id);

                auto el_type = exII::element_name(cell.type());
                auto n = cell.num_vertices() * elems.size();
                std::vector<int> connect;
                connect.reserve(n);
                for (auto & cell_id : elems) {
                    auto & el = mesh.element(cell_id);
                    for (int j = 0; j < el.ids().size(); j++)
                        connect.push_back(el.vertex_id(j) + 1);
                }
                this->exo.write_block(blk_id, el_type, elems.size(), connect);
                blk_id++;
            }
        }
    }
    else {
        // NOTE: krado allows to have different cell types in a single side set, but not exodusII.
        // So, we need to filter on both cell set id and cell type, i.e. put all cells of the same
        // type in the same block.
        // Currently, we just assume cell sets are homogeneous in terms of cell type.

        std::map<int, std::vector<Element>> elem_blks;
        std::map<int, std::string> elem_blk_names;
        int exii_idx = 1;
        for (auto & cell_id : mesh.cell_ids()) {
            this->exii_elem_ids[cell_id] = exii_idx++;
            auto & cell = mesh.element(cell_id);
            auto marker = cell.marker();
            elem_blks[marker].push_back(cell);
            if (elem_blk_names.find(marker) == elem_blk_names.end()) {
                auto blk_name = mesh.cell_set_name(marker);
                elem_blk_names[marker] = blk_name;
            }
        }

        std::vector<std::string> blk_names;
        for (auto & [blk_id, elems] : elem_blks) {
            if (!elems.empty()) {
                auto el_type = exII::element_name(elems[0].type());
                auto n = elems[0].num_vertices() * elems.size();
                std::vector<int> connect;
                connect.reserve(n);
                for (auto & el : elems) {
                    for (int j = 0; j < el.ids().size(); j++)
                        connect.push_back(el.vertex_id(j) + 1);
                }
                this->exo.write_block(blk_id, el_type, elems.size(), connect);
                blk_names.push_back(elem_blk_names[blk_id]);
            }
        }

        if (!blk_names.empty())
            this->exo.write_block_names(blk_names);
    }
}

void
ExodusIIFile::write_side_sets(const Mesh & mesh)
{
    std::vector<std::string> side_sets_names;
    auto set_ids = mesh.side_set_ids();
    for (auto & sid : set_ids) {
        std::vector<int> elems;
        std::vector<int> sides;
        auto & entries = mesh.side_set(sid);
        for (auto & en : entries) {
            elems.push_back(this->exii_elem_ids.at(en.elem));
            auto et = mesh.element_type(en.elem);
            sides.push_back(exII::local_side_index(et, en.side));
        }
        this->exo.write_side_set(sid, elems, sides);
        side_sets_names.push_back(mesh.side_set_name(sid));
    }

    this->exo.write_side_set_names(side_sets_names);
}

} // namespace krado
