// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/export.h"
#include "krado/mesh.h"
#include "krado/exception.h"
#include "krado/config.h"
#include "exodusIIcpp/exodusIIcpp.h"
#include "fmt/format.h"
#include "fmt/chrono.h"

namespace krado {

namespace {

const char *
get_exodusii_name(MeshElement::Type t)
{
    switch (t) {
    case MeshElement::LINE2:
        return "BAR2";
    case MeshElement::TRI3:
        return "TRI3";
    case MeshElement::TETRA4:
        return "TET4";
    }
    throw Exception("Unsupported element type {}.", t);
}

class ExodusIIWriter {
public:
    explicit ExodusIIWriter(const Mesh & mesh) : mesh(mesh), dim(-1) {}

    void
    write(const std::string & file_name)
    {
        this->exo.create(file_name);
        auto sz = this->mesh.bounding_box().size();
        if ((sz[0] > 0) && (sz[1] < 1e-15) && (sz[2] < 1e-15))
            this->dim = 1;
        else if ((sz[0] > 0) && (sz[1] > 0) && (sz[2] < 1e-15))
            this->dim = 2;
        else if ((sz[0] > 0) && (sz[1] > 0) && (sz[2] > 0))
            this->dim = 3;
        else
            throw Exception("Unusual mesh, unable to write.");
        preprocess_mesh();

        int n_nodes = (int) this->mesh.points().size();
        int n_elems = (int) this->mesh.elements().size();
        int n_elem_blks = this->elem_blks.size();
        int n_node_sets = 0, n_side_sets = 0;
        this->exo.init("", this->dim, n_nodes, n_elems, n_elem_blks, n_node_sets, n_side_sets);

        write_info();
        write_coords();
        write_elements();

        this->exo.close();
    }

private:
    void
    write_info()
    {
        std::time_t now = std::time(nullptr);
        std::string datetime = fmt::format("{:%d %b %Y, %H:%M:%S}", fmt::localtime(now));

        std::vector<std::string> info(1);
        info[0] = fmt::format("Created by krado v{} on {}", KRADO_VERSION, datetime);
        this->exo.write_info(info);
    }

    void
    write_coords()
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
    write_elements()
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
    preprocess_mesh()
    {
        for (auto & pt : this->mesh.points()) {
            if (this->dim >= 1)
                this->x.push_back(pt.x);
            if (this->dim >= 2)
                this->y.push_back(pt.y);
            if (this->dim >= 3)
                this->z.push_back(pt.z);
        }

        for (auto & el : this->mesh.elements()) {
            auto marker = el.marker();
            this->elem_blks[marker].push_back(el);
        }
    }

    const Mesh & mesh;
    exodusIIcpp::File exo;
    int dim;
    std::vector<double> x, y, z;
    std::map<int, std::vector<MeshElement>> elem_blks;
};

} // namespace

void
write_exodusii(const Mesh & mesh, const std::string & file_name)
{
    try {
        ExodusIIWriter writer(mesh);
        writer.write(file_name);
    }
    catch (exodusIIcpp::Exception & e) {
        throw Exception("Failed to write '{}'.", file_name);
    }
}

} // namespace krado
