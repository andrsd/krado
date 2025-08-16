// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

// Print information about mesh

#include "krado/mesh.h"
#include "krado/log.h"
#include "krado/exodusii_file.h"
#include "krado/utils.h"
#include "cxxopts/cxxopts.hpp"
#include "fmt/core.h"
#include <string>
#include <algorithm>
#include <map>
#include <exception>

cxxopts::Options
build_options()
{
    cxxopts::Options options("krado-info", "Display information about a mesh");
    // clang-format off
    options.add_options()
        ("filename", "The mesh file name", cxxopts::value<std::string>())
        ("h,help", "Print usage")
    ;
    // clang-format on
    options.parse_positional({ "filename" });
    return options;
}

krado::Mesh
load_exodus_file(const std::string & filename)
{
    krado::ExodusIIFile file(filename);
    return file.read();
}

std::map<krado::ElementType, std::size_t>
element_counts(const krado::Mesh & mesh, krado::marker_t cell_set_id)
{
    const auto & cells = mesh.cell_set(cell_set_id);
    std::map<krado::ElementType, std::size_t> counts;
    for (auto & id : cells) {
        auto etyp = mesh.element_type(id);
        counts[etyp]++;
    }
    return counts;
}

void
print_cell_set_info(const krado::Mesh & mesh)
{
    const auto & cell_set_ids = mesh.cell_set_ids();
    if (cell_set_ids.size() > 0) {
        fmt::print("\n");
        fmt::print("Cell sets [{}]:\n", cell_set_ids.size());
        std::size_t wd_id = 1;
        std::size_t wd_name = 1;
        std::size_t wd_num = 1;
        for (const auto & id : cell_set_ids) {
            wd_id = std::max(wd_id, fmt::format("{}", id).size());
            auto name = mesh.cell_set_name(id);
            if (name.size() == 0)
                name = "<no name>";
            wd_name = std::max(wd_name, fmt::format("{}", name).size());
            const auto & cell_set = mesh.cell_set(id);
            wd_num =
                std::max(wd_num,
                         fmt::format("{}", krado::utils::human_number(cell_set.size())).size());
        }
        wd_name++;

        for (const auto & id : cell_set_ids) {
            auto elem_counts = element_counts(mesh, id);

            fmt::print("- {:>{}}: ", id, wd_id);

            auto name = mesh.cell_set_name(id);
            if (name.size() == 0)
                name = "<no name>";
            fmt::print("{:<{}} ", name, wd_name);

            const auto & cell_set = mesh.cell_set(id);
            fmt::print("{:>{}} elements total",
                       krado::utils::human_number(cell_set.size()),
                       wd_num);
            fmt::print(" (");
            if (elem_counts.size() == 1) {
                auto etyp = elem_counts.begin()->first;
                fmt::print("{}", krado::utils::to_str(etyp));
            }
            else {
                int i = 0;
                for (auto & [etyp, cnt] : elem_counts) {
                    if (i > 0)
                        fmt::print(", ");
                    fmt::print("{}: {}",
                               krado::utils::to_str(etyp),
                               krado::utils::human_number(cnt));
                    i++;
                }
            }
            fmt::print(")");
            fmt::print("\n");
        }
    }
}

void
print_side_set_info(const krado::Mesh & mesh)
{
    // side sets
    const auto & side_set_ids = mesh.side_set_ids();
    if (side_set_ids.size() > 0) {
        std::size_t wd_id = 1;
        std::size_t wd_name = 1;
        std::size_t wd_num = 1;
        for (const auto & id : side_set_ids) {
            wd_id = std::max(wd_id, fmt::format("{}", id).size());
            auto name = mesh.side_set_name(id);
            if (name.size() == 0)
                name = "<no name>";
            wd_name = std::max(wd_name, fmt::format("{}", name).size());
            const auto & side_set = mesh.side_set(id);
            wd_num =
                std::max(wd_num,
                         fmt::format("{}", krado::utils::human_number(side_set.size())).size());
        }
        wd_name++;

        fmt::print("\n");
        fmt::print("Side sets [{}]:\n", side_set_ids.size());

        for (const auto & id : side_set_ids) {
            fmt::print("- {:>{}}: ", id, wd_id);
            auto name = mesh.side_set_name(id);
            if (name.size() == 0)
                name = "<no name>";
            fmt::print("{:<{}} ", name, wd_name);

            const auto & side_set = mesh.side_set(id);
            fmt::print("{:>{}} sides\n", krado::utils::human_number(side_set.size()), wd_num);
        }
    }
}

void
print_mesh_info(const std::string & filename)
{
    fmt::print("Reading file: {}...", filename);
    std::fflush(stdout);
    auto mesh = load_exodus_file(filename);
    fmt::print(" done\n");

    fmt::print("\n");
    fmt::print("Global:\n");
    fmt::print("- {} elements\n", krado::utils::human_number(mesh.elements().size()));
    fmt::print("- {} nodes\n", krado::utils::human_number(mesh.points().size()));

    print_cell_set_info(mesh);
    print_side_set_info(mesh);
}

int
main(int argc, char ** argv)
{
    try {
        krado::Log::set_verbosity(0);
        auto options = build_options();
        auto result = options.parse(argc, argv);
        if (result["filename"].count())
            print_mesh_info(result["filename"].as<std::string>());
        else {
            fmt::print("{}\n", options.help());
        }
        return 0;
    }
    catch (const std::exception & e) {
        fmt::print("Error: {}\n", e.what());
        return 1;
    }
}
