// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/dagmc_file.h"
#include "krado/element.h"
#include "krado/geom_model.h"
#include "krado/exception.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_volume.h"
#include "krado/types.h"
#ifdef KRADO_WITH_MOAB
    #include "MBTagConventions.hpp"
    #include "moab/Core.hpp"
#endif

namespace krado {

#ifdef KRADO_WITH_MOAB

    #define CHECK_MOAB(err)                                                            \
        {                                                                              \
            moab::ErrorCode rval = (err);                                              \
            if (rval != moab::MB_SUCCESS)                                              \
                throw Exception("MOAB error: {}", this->core_.get_error_string(rval)); \
        }

namespace {

template <typename>
struct is_std_vector : std::false_type {};

template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type {};

template <typename>
struct is_std_array : std::false_type {};

template <typename T, typename N>
struct is_std_array<std::vector<T, N>> : std::true_type {};

template <typename T>
moab::DataType get_moab_datatype();

template <>
moab::DataType
get_moab_datatype<int>()
{
    return moab::MB_TYPE_INTEGER;
}

template <>
moab::DataType
get_moab_datatype<double>()
{
    return moab::MB_TYPE_DOUBLE;
}

//

template <ElementType ET>
moab::EntityType get_moab_element_type();

template <>
moab::EntityType
get_moab_element_type<ElementType::TRI3>()
{
    return moab::MBTRI;
}

template <>
moab::EntityType
get_moab_element_type<ElementType::QUAD4>()
{
    return moab::MBQUAD;
}

} // namespace

class MOABFile {
public:
    MOABFile();

    void write(const std::string & file_name);

    void add_surface(const MeshSurface & surface);
    void add_volume(const MeshVolume & volume);
    void add_parent_child(const MeshVolume & volume, const MeshSurface & surface);
    void add_surface_triangulation(const MeshSurface & surface);
    void add_surface_senses(const std::map<int, std::vector<int>> & surfaces_with_volumes);
    void add_group(const std::string & material, const std::vector<MeshVolume *> & volumes);
    void gather_entities();

private:
    void create_tags();

    template <typename T>
    moab::Tag
    tag_get_handle(const std::string & name,
                   krado::Flags<moab::TagType> tags,
                   const void * default_value = nullptr)
    {
        auto dt = get_moab_datatype<T>();
        moab::Tag tag;
        CHECK_MOAB(
            this->core_.tag_get_handle(name.c_str(), 1, dt, tag, tags.as_uint(), default_value));
        return tag;
    }

    moab::EntityHandle create_meshset(Flags<moab::EntitySetProperty> flags, int start_id = 0);

    template <typename T>
    void
    tag_set_data(moab::Tag tag_handle, const moab::EntityHandle entity_handle, T value)
    {
        if constexpr (is_std_array<T>::value) {
            CHECK_MOAB(
                this->core_.tag_set_data(tag_handle, &entity_handle, value.size(), value.data()));
        }
        else {
            CHECK_MOAB(this->core_.tag_set_data(tag_handle, &entity_handle, 1, &value));
        }
    }

    moab::EntityHandle create_vertex(const MeshVertexAbstract * vtx);

    template <ElementType ET>
    moab::EntityHandle
    create_element(std::array<moab::EntityHandle, ElementSelector<ET>::N_VERTICES> & corners)
    {
        moab::EntityHandle h;
        auto type = get_moab_element_type<ET>();
        CHECK_MOAB(this->core_.create_element(type, corners.data(), corners.size(), h));
        return h;
    }

    moab::Core core_;
    moab::Tag geom_tag_;
    moab::Tag id_tag_;
    moab::Tag name_tag_;
    moab::Tag category_tag_;
    moab::Tag faceting_tol_tag_;
    moab::Tag geometry_resabs_tag_;
    moab::Tag surf_sense_tag_;
    /// file set for storage of tolerance values
    moab::EntityHandle file_set_;

    double faceting_tol_;

    std::map<int, moab::EntityHandle> surface_sets_;
    std::map<int, moab::EntityHandle> volume_sets_;
    std::map<int, moab::EntityHandle> group_sets_;
};

MOABFile::MOABFile() :
    geom_tag_(nullptr),
    id_tag_(nullptr),
    name_tag_(nullptr),
    category_tag_(nullptr),
    faceting_tol_tag_(nullptr),
    geometry_resabs_tag_(nullptr),
    surf_sense_tag_(nullptr),
    faceting_tol_(1e-3)
{
    create_tags();
    this->file_set_ = create_meshset(0);
}

moab::EntityHandle
MOABFile::create_meshset(Flags<moab::EntitySetProperty> flags, int start_id)
{
    auto options = flags.as_uint();
    moab::EntityHandle set;
    CHECK_MOAB(this->core_.create_meshset(options, set, start_id));
    return set;
}

void
MOABFile::create_tags()
{
    int negone = -1;
    this->geom_tag_ =
        tag_get_handle<int>(GEOM_DIMENSION_TAG_NAME,
                            moab::MB_TAG_SPARSE | moab::MB_TAG_ANY | moab::MB_TAG_CREAT,
                            &negone);
    int zero = 0;
    this->id_tag_ =
        tag_get_handle<int>(GLOBAL_ID_TAG_NAME, moab::MB_TAG_DENSE | moab::MB_TAG_ANY, &zero);

    CHECK_MOAB(this->core_.tag_get_handle(NAME_TAG_NAME,
                                          NAME_TAG_SIZE,
                                          moab::MB_TYPE_OPAQUE,
                                          this->name_tag_,
                                          (unsigned) moab::MB_TAG_SPARSE | moab::MB_TAG_ANY |
                                              moab::MB_TAG_CREAT));

    CHECK_MOAB(this->core_.tag_get_handle(CATEGORY_TAG_NAME,
                                          CATEGORY_TAG_SIZE,
                                          moab::MB_TYPE_OPAQUE,
                                          this->category_tag_,
                                          (unsigned) moab::MB_TAG_SPARSE | moab::MB_TAG_CREAT));

    this->faceting_tol_tag_ =
        tag_get_handle<double>("FACETING_TOL", moab::MB_TAG_SPARSE | moab::MB_TAG_CREAT);

    this->geometry_resabs_tag_ =
        tag_get_handle<double>("GEOMETRY_RESABS", moab::MB_TAG_SPARSE | moab::MB_TAG_CREAT);

    CHECK_MOAB(this->core_.tag_get_handle("GEOM_SENSE_2",
                                          2,
                                          moab::MB_TYPE_HANDLE,
                                          this->surf_sense_tag_,
                                          (unsigned) moab::MB_TAG_SPARSE | moab::MB_TAG_CREAT));
}

void
MOABFile::write(const std::string & file_name)
{
    tag_set_data(this->faceting_tol_tag_, this->file_set_, this->faceting_tol_);
    // tag_set_data(this->geometry_resabs_tag_, this->file_set_, GEOMETRY_RESABS);

    CHECK_MOAB(this->core_.write_file(file_name.c_str()));
}

void
MOABFile::add_surface(const MeshSurface & surface)
{
    int id = surface.id();
    auto it = this->surface_sets_.find(id);
    if (it == this->surface_sets_.end()) {
        auto face_set = create_meshset(moab::MESHSET_SET);
        this->surface_sets_[id] = face_set;

        tag_set_data(this->id_tag_, face_set, id);
        tag_set_data(this->geom_tag_, face_set, 2);
        tag_set_data(this->category_tag_, face_set, "Surface");
    }
}

void
MOABFile::add_volume(const MeshVolume & volume)
{
    int id = volume.id();
    auto it = this->volume_sets_.find(id);
    if (it == this->volume_sets_.end()) {
        auto volume_set = create_meshset(moab::MESHSET_SET);
        this->volume_sets_[id] = volume_set;

        tag_set_data(this->id_tag_, volume_set, id);
        tag_set_data(this->geom_tag_, volume_set, 3);
        tag_set_data(this->category_tag_, volume_set, "Volume");
    }
}

void
MOABFile::add_group(const std::string & material, const std::vector<MeshVolume *> & volumes)
{
    auto group_id = this->group_sets_.size();
    auto group_set = create_meshset(moab::MESHSET_SET);
    tag_set_data(this->category_tag_, group_set, "Group");
    tag_set_data(this->id_tag_, group_set, group_id);

    auto name = fmt::format("mat:{}", material);
    if (name.size() >= NAME_TAG_SIZE)
        name = name.substr(0, NAME_TAG_SIZE);
    tag_set_data(this->name_tag_, group_set, name);

    moab::Range entities;
    for (auto & vol : volumes) {
        auto id = vol->id();
        auto volume_set = this->volume_sets_.at(id);
        entities.insert(volume_set);
    }
    CHECK_MOAB(this->core_.add_entities(group_set, entities));
    this->group_sets_.insert({ group_id, group_set });
}

void
MOABFile::add_parent_child(const MeshVolume & volume, const MeshSurface & surface)
{
    auto vol_set = this->volume_sets_.at(volume.id());
    auto face_set = this->surface_sets_.at(surface.id());
    CHECK_MOAB(this->core_.add_parent_child(vol_set, face_set));
}

moab::EntityHandle
MOABFile::create_vertex(const MeshVertexAbstract * vtx)
{
    auto pt = vtx->point();
    double coords[] = { pt.x, pt.y, pt.z };
    moab::EntityHandle h;
    CHECK_MOAB(this->core_.create_vertex(coords, h));
    return h;
}

void
MOABFile::add_surface_triangulation(const MeshSurface & surface)
{
    if (!surface.is_meshed())
        return;

    auto face_set = this->surface_sets_.at(surface.id());

    moab::Range vertices;
    std::map<MeshVertexAbstract *, moab::EntityHandle> vertex_map;
    for (auto & vtx : surface.all_vertices()) {
        auto h = create_vertex(vtx);
        vertex_map[vtx] = h;
        vertices.insert(h);
    }
    CHECK_MOAB(this->core_.add_entities(face_set, vertices));

    moab::Range facets;
    for (auto & tri : surface.triangles()) {
        std::array<moab::EntityHandle, Tri3::N_VERTICES> corners;
        for (int i = 0; i < Tri3::N_VERTICES; ++i)
            corners[i] = vertex_map[tri.vertex(i)];
        auto h = create_element<ElementType::TRI3>(corners);
        facets.insert(h);
    }
    CHECK_MOAB(this->core_.add_entities(face_set, facets));
}

void
MOABFile::add_surface_senses(const std::map<int, std::vector<int>> & surfaces_with_volumes)
{
    assert(this->surface_sets_.size() > 0);
    assert(this->volume_sets_.size() > 0);

    std::map<int, bool> added_surface_ids;
    for (auto & [surf_id, vol_ids] : surfaces_with_volumes) {
        auto face_set = this->surface_sets_[surf_id];
        std::array<moab::EntityHandle, 2> sense_data;
        if (added_surface_ids.find(surf_id) == added_surface_ids.end()) {
            if (vol_ids.size() == 2)
                sense_data = { this->volume_sets_[vol_ids[1]], this->volume_sets_[vol_ids[0]] };
            else if (vol_ids.size() == 1)
                sense_data = { this->volume_sets_[vol_ids[0]], 0 };
            else
                throw Exception("Surface {} is shared with {} volumes. This is unusual.",
                                surf_id,
                                vol_ids.size());
            added_surface_ids[surf_id] = true;
        }
        else
            sense_data = { this->volume_sets_[vol_ids[0]], this->volume_sets_[vol_ids[1]] };
        tag_set_data(this->surf_sense_tag_, face_set, sense_data);
    }
}

void
MOABFile::gather_entities()
{
    moab::Range new_ents;
    CHECK_MOAB(this->core_.get_entities_by_handle(0, new_ents));

    moab::Range gather_ents;
    CHECK_MOAB(this->core_.get_entities_by_handle(this->file_set_, gather_ents));
    if (gather_ents.size() > 0)
        throw Exception("file set already contains entities");

    CHECK_MOAB(this->core_.add_entities(this->file_set_, new_ents));
}

///

DAGMCFile::DAGMCFile(const std::string & file_name) : file_name_(file_name) {}

void
DAGMCFile::write(const GeomModel & model)
{
    MOABFile file;

    for (auto [vol_id, volume] : model.volumes())
        file.add_volume(volume);
    for (auto [surf_id, surface] : model.surfaces())
        file.add_surface(surface);

    // create topology
    for (auto [vol_id, volume] : model.volumes())
        for (auto & surface : volume.surfaces()) {
            assert(surface != nullptr);
            file.add_parent_child(volume, *surface);
        }

    // add surface meshes
    for (auto [surf_id, surface] : model.surfaces())
        file.add_surface_triangulation(surface);

    // surface senses
    std::map<int, std::vector<int>> surfaces_with_volumes;
    for (auto [vol_id, volume] : model.volumes())
        for (auto & surface : volume.surfaces())
            surfaces_with_volumes[surface->id()].push_back(vol_id);
    file.add_surface_senses(surfaces_with_volumes);

    // create groups
    std::map<std::string, std::vector<MeshVolume *>> groups;
    for (auto [vol_id, volume] : model.volumes()) {
        auto & gvol = volume.geom_volume();
        if (gvol.has_material()) {
            auto material_name = gvol.material();
            groups[material_name].push_back(&volume);
        }
        else
            throw Exception("Volume {} has no material associated", vol_id);
    }
    for (auto & [group_name, volumes] : groups)
        file.add_group(group_name, volumes);

    file.gather_entities();

    file.write(this->file_name_);
}

#else

DAGMCFile::DAGMCFile(const std::string & file_name) : file_name_(file_name)
{
    throw Exception("krado was not build with MOAB support. Re-configure with -DKRADO_WITH_MOAB to "
                    "enable support for DAGMC files.");
}

void
DAGMCFile::write(const GeomModel & model)
{
    // do nothing
}

#endif

} // namespace krado
