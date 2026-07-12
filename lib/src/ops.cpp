// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/ops.h"
#include "krado/element.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_shell.h"
#include "krado/geom_volume.h"
#include "krado/exception.h"
#include "krado/log.h"
#include "krado/mesh.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_element.h"
#include "krado/types.h"
#include "krado/vector.h"
#include "krado/wire.h"
#include "krado/plane.h"
#include "krado/axis1.h"
#include "krado/range.h"
#include "krado/timer.h"
#include "Geom_TrimmedCurve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "BRepBuilderAPI_Sewing.hxx"
#include "BRep_Tool.hxx"
#include "BRepAlgoAPI_Fuse.hxx"
#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Common.hxx"
#include "BRepAlgoAPI_Section.hxx"
#include "BRepAlgo_NormalProjection.hxx"
#include "BRepFeat_SplitShape.hxx"
#include "BRepFeat_MakeCylindricalHole.hxx"
#include "BRepFilletAPI_MakeFillet.hxx"
#include "BRepOffsetAPI_MakeThickSolid.hxx"
#include "BRepOffsetAPI_DraftAngle.hxx"
#include "BRepPrimAPI_MakePrism.hxx"
#include "BRepPrimAPI_MakeRevol.hxx"
#include "BRepOffsetAPI_MakePipe.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepAlgoAPI_Splitter.hxx"
#include "TopoDS_Edge.hxx"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include <set>

namespace krado {

GeomShape
translate(const GeomShape & shape, Vector v)
{
    gp_Trsf trsf;
    trsf.SetTranslation(v);
    BRepBuilderAPI_Transform brep_trsf(shape, trsf);
    return GeomShape(brep_trsf.Shape());
}

GeomShape
translate(const GeomShape & shape, Point p1, Point p2)
{
    gp_Trsf trsf;
    trsf.SetTranslation(p1, p2);
    BRepBuilderAPI_Transform brep_trsf(shape, trsf);
    return GeomShape(brep_trsf.Shape());
}

GeomShape
scale(const GeomShape & shape, double s)
{
    gp_Trsf trsf;
    trsf.SetScaleFactor(s);
    BRepBuilderAPI_Transform brep_trsf(shape, trsf);
    return GeomShape(brep_trsf.Shape());
}

GeomShape
mirror(const GeomShape & shape, const Axis1 & axis)
{
    gp_Trsf trsf;
    trsf.SetMirror(axis);
    BRepBuilderAPI_Transform brep_trsf(shape, trsf);
    return GeomShape(brep_trsf.Shape());
}

Ptr<Mesh>
mirror(Ptr<const Mesh> mesh, const Axis2 & axis)
{
    return mesh->mirrored(axis);
}

std::tuple<GeomCurve, GeomCurve>
split_curve(const GeomCurve & curve, Standard_Real split_param)
{
    double umin, umax;
    Handle(Geom_Curve) orig_curve = BRep_Tool::Curve(curve, umin, umax);
    if (split_param < umin || split_param > umax)
        throw Exception("Split parameter {} out of range [{}, {}]", split_param, umin, umax);

    Handle(Geom_Curve) first_part = new Geom_TrimmedCurve(orig_curve, umin, split_param);
    Handle(Geom_Curve) second_part = new Geom_TrimmedCurve(orig_curve, split_param, umax);

    auto first_edge = BRepBuilderAPI_MakeEdge(first_part);
    auto second_edge = BRepBuilderAPI_MakeEdge(second_part);

    return { GeomCurve(first_edge), GeomCurve(second_edge) };
}

GeomShell
imprint(const GeomSurface & surface, const GeomCurve & curve)
{
    BRepAlgo_NormalProjection projection(surface);
    projection.Add(curve);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto surface failed.");

    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());

    BRepFeat_SplitShape splitter(surface);
    splitter.Add(seq);
    splitter.Build();
    if (splitter.IsDone()) {
        auto split_shape = splitter.Shape();
        if (split_shape.ShapeType() == TopAbs_SHELL)
            return GeomShell(TopoDS::Shell(split_shape));
        else
            throw Exception("Imprint did not produce a shell.");
    }
    else
        throw Exception("Imprint surface with curve failed.");
}

GeomVolume
imprint(const GeomVolume & volume, const GeomCurve & curve)
{
    BRepAlgo_NormalProjection projection(volume);
    // arbitrary distance limit, shapes must be close together
    projection.SetMaxDistance(1e-10);
    projection.Add(curve);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto volume failed.");

    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());
    if (seq.Size() == 0)
        throw Exception("Imprint: projection of curve onto volume yield empty result.");

    BRepFeat_SplitShape splitter(volume);
    splitter.Add(seq);
    splitter.Build();
    if (!splitter.IsDone())
        throw Exception("Imprint volume with curve failed.");

    auto result = splitter.Shape();
    return GeomVolume(TopoDS::Solid(result));
}

GeomVolume
imprint(const GeomVolume & volume, const GeomVolume & other)
{
    TopTools_ListOfShape args;
    args.Append(volume);

    BRepAlgo_NormalProjection projection(volume);
    // arbitrary distance limit, shapes must be close together
    projection.SetMaxDistance(1e-10);
    TopExp_Explorer vol_exp;
    TopTools_DataMapOfShapeInteger crv_id;
    int id = 0;
    for (vol_exp.Init(other, TopAbs_EDGE); vol_exp.More(); vol_exp.Next()) {
        auto edge = TopoDS::Edge(vol_exp.Current());
        if (!crv_id.IsBound(edge)) {
            crv_id.Bind(edge, ++id);
            projection.Add(edge);
        }
    }
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Projection of volume onto volume failed.");
    TopTools_ListOfShape tools;
    projection.BuildWire(tools);
    if (tools.IsEmpty())
        throw Exception("Projection of volume onto volume yield empty result.");

    BRepAlgoAPI_Splitter splitter;
    splitter.SetArguments(args);
    splitter.SetTools(tools);
    splitter.Build();
    if (!splitter.IsDone())
        throw Exception("Imprint volume with volume failed.");

    auto result = splitter.Shape();
    return GeomVolume(TopoDS::Solid(result));
}

std::map<Marker, double>
compute_volume(const Mesh & mesh)
{
    auto element_volume = [&](const Element & elem) {
        switch (elem.type()) {
        case ElementType::LINE2: {
            auto idxs = elem.indices();
            auto vec = mesh.point(idxs[1]) - mesh.point(idxs[0]);
            return vec.magnitude();
        }

        case ElementType::TRI3: {
            auto idxs = elem.indices();
            auto va = mesh.point(idxs[1]) - mesh.point(idxs[0]);
            auto vb = mesh.point(idxs[2]) - mesh.point(idxs[0]);
            return 0.5 * cross_product(va, vb).magnitude();
        }

        case ElementType::TETRA4: {
            auto idxs = elem.indices();
            auto va = mesh.point(idxs[1]) - mesh.point(idxs[0]);
            auto vb = mesh.point(idxs[2]) - mesh.point(idxs[0]);
            auto vc = mesh.point(idxs[3]) - mesh.point(idxs[0]);
            return std::abs(dot_product(va, cross_product(vb, vc))) / 6.;
        }

        default:
            throw Exception("compute_volume: Unsupported element type {}", elem.type());
        }
    };

    auto cellsets_ids = mesh.cell_set_ids();
    if (cellsets_ids.empty()) {
        double volume = 0.;
        for (auto & elem : mesh.elements())
            volume += element_volume(elem);
        return { std::pair(0, volume) };
    }
    else {
        std::map<Marker, double> vols_per_cellset;
        for (auto csid : cellsets_ids) {
            auto & volume = vols_per_cellset[csid];
            for (auto & cid : mesh.cell_set(csid)) {
                auto & elem = mesh.element(cid);
                volume += element_volume(elem);
            }
        }
        return vols_per_cellset;
    }
}

std::map<Marker, double>
compute_volume(Ptr<const Mesh> mesh)
{
    return compute_volume(*mesh);
}

Ptr<Mesh>
combine(const std::vector<Ptr<Mesh>> & parts)
{
    Index n_total_elems = 0;
    Index n_total_points = 0;
    // how much we shift element and point indices per mesh part
    std::vector<Index> elem_shift;
    elem_shift.reserve(parts.size());
    std::vector<Index> pts_shift;
    pts_shift.reserve(parts.size());
    for (auto & p : parts) {
        elem_shift.push_back(n_total_elems);
        pts_shift.push_back(n_total_points);
        n_total_elems += p->num_elements();
        n_total_points += p->num_points();
    }

    // combine points and elements
    std::vector<Point> points;
    std::vector<Element> elements;
    points.reserve(n_total_points);
    elements.reserve(n_total_elems);
    for (auto & p : parts) {
        points.insert(points.end(), p->points().begin(), p->points().end());
        elements.insert(elements.end(), p->elements().begin(), p->elements().end());
    }
    // shift points
    for (std::size_t i = 0, k = 0; i < parts.size(); ++i) {
        auto & p = parts[i];
        for (std::size_t j = 0; j < p->num_elements(); ++j, ++k) {
            auto & elem = elements[k];
            elem.shift(pts_shift[i]);
        }
    }

    // merge cell sets
    std::unordered_map<Marker, std::size_t> cell_sets_size;
    for (auto & p : parts) {
        for (auto id : p->cell_set_ids()) {
            auto cell_set = p->cell_set(id);
            auto it = cell_sets_size.find(id);
            if (it == cell_sets_size.end())
                cell_sets_size[id] = cell_set.size();
            else
                cell_sets_size[id] += cell_set.size();
        }
    }
    std::map<Marker, std::string> cell_set_names;
    std::map<Marker, std::vector<Index>> cell_sets;
    for (auto & [id, size] : cell_sets_size)
        cell_sets[id].reserve(size);
    for (auto i : make_range(parts.size())) {
        auto & p = parts[i];
        for (auto & id : p->cell_set_ids()) {
            auto name = p->cell_set_name(id);
            if (cell_set_names.find(id) == cell_set_names.end()) {
                if (name.has_value())
                    cell_set_names[id] = name.value();
            }

            auto cell_set = p->cell_set(id);
            for (auto & c : cell_set)
                cell_sets[id].push_back(c + elem_shift[i]);
        }
    }

    // merge side sets
    std::map<Marker, std::string> side_set_names;
    std::map<Marker, std::vector<SideEntry>> side_sets;
    {
        std::unordered_map<Marker, std::size_t> side_sets_size;
        for (auto & p : parts) {
            for (auto id : p->side_set_ids()) {
                auto side_set = p->side_set(id);
                auto it = side_sets_size.find(id);
                if (it == side_sets_size.end())
                    side_sets_size[id] = side_set.size();
                else
                    side_sets_size[id] += side_set.size();
            }
        }

        for (auto & [id, n] : side_sets_size)
            side_sets[id].reserve(n);
        for (auto i : make_range(parts.size())) {
            auto & p = parts[i];
            for (auto & id : p->side_set_ids()) {
                auto name = p->side_set_name(id);
                if (side_set_names.find(id) == side_set_names.end()) {
                    if (name.has_value())
                        side_set_names[id] = name.value();
                }

                auto side_set = p->side_set(id);
                for (auto & c : side_set)
                    side_sets[id].push_back({ c.elem + elem_shift[i], c.side });
            }
        }
    }

    // merge node sets
    std::map<Marker, std::string> node_set_names;
    std::map<Marker, std::vector<Index>> node_sets;
    {
        std::unordered_map<Marker, std::size_t> node_sets_size;
        for (auto & p : parts) {
            for (auto id : p->node_set_ids()) {
                auto node_set = p->node_set(id);
                auto it = node_sets_size.find(id);
                if (it == node_sets_size.end())
                    node_sets_size[id] = node_set.size();
                else
                    node_sets_size[id] += node_set.size();
            }
        }

        for (auto & [id, n] : node_sets_size)
            node_sets[id].reserve(n);
        for (auto i : make_range(parts.size())) {
            auto & p = parts[i];
            for (auto & id : p->node_set_ids()) {
                auto name = p->node_set_name(id);
                if (node_set_names.find(id) == node_set_names.end()) {
                    if (name.has_value())
                        node_set_names[id] = name.value();
                }

                auto node_set = p->node_set(id);
                for (auto & c : node_set)
                    node_sets[id].push_back(c + pts_shift[i]);
            }
        }
    }

    auto mesh = Ptr<Mesh>::alloc(points, elements);
    for (auto & [id, cset] : cell_sets)
        mesh->set_cell_set(id, cset);
    for (auto & [id, name] : cell_set_names)
        mesh->set_cell_set_name(id, name);

    for (auto & [id, sset] : side_sets)
        mesh->set_side_set(id, sset);
    for (auto & [id, name] : side_set_names)
        mesh->set_side_set_name(id, name);

    for (auto & [id, nset] : node_sets)
        mesh->set_node_set(id, nset);
    for (auto & [id, name] : node_set_names)
        mesh->set_node_set_name(id, name);

    return mesh;
}

GeomShape
fuse(const GeomShape & shape, const GeomShape & tool, bool simplify)
{
    BRepAlgoAPI_Fuse alg(shape, tool);
    alg.Build();
    if (simplify)
        alg.SimplifyResult();
    if (!alg.IsDone())
        throw Exception("Objects were not fused");
    return GeomShape(alg.Shape());
}

GeomShape
fuse(const std::vector<GeomShape> & shapes, bool simplify)
{
    if (shapes.empty())
        throw Exception("No shapes to fuse");

    if (shapes.size() == 1)
        throw Exception("Only one shape provided");

    TopoDS_Shape sh = shapes[0];
    for (auto i : make_range(1, shapes.size())) {
        BRepAlgoAPI_Fuse alg(sh, shapes[i]);
        alg.Build();
        if (simplify)
            alg.SimplifyResult();
        if (!alg.IsDone())
            throw Exception("Objects were not fused");
        sh = alg.Shape();
    }

    return GeomShape(sh);
}

GeomShape
cut(const GeomShape & shape, const GeomShape & tool)
{
    BRepAlgoAPI_Cut alg(shape, tool);
    alg.Build();
    if (!alg.IsDone())
        throw Exception("Object was not cut");
    return GeomShape(alg.Shape());
}

GeomShape
intersect(const GeomShape & shape, const GeomShape & tool)
{
    BRepAlgoAPI_Common alg(shape, tool);
    alg.Build();
    if (!alg.IsDone())
        throw Exception("Object was not intersected");
    return GeomShape(alg.Shape());
}

GeomShape
fillet(const GeomShape & shape, const std::vector<GeomCurve> & edges, double radius)
{
    BRepFilletAPI_MakeFillet flt(shape);
    for (auto & e : edges)
        flt.Add(radius, e);
    return GeomShape(flt.Shape());
}

GeomShape
hollow(const GeomShape & shape,
       const std::vector<GeomSurface> & faces_to_remove,
       double thickness,
       double tolerance)
{
    TopTools_ListOfShape rem_faces;
    for (auto & face : faces_to_remove)
        rem_faces.Append(face);

    BRepOffsetAPI_MakeThickSolid result;
    result.MakeThickSolidByJoin(shape, rem_faces, thickness, tolerance);
    result.Build();
    if (!result.IsDone())
        throw Exception("hollow failed");
    return GeomShape(result.Shape());
}

GeomShape
extrude(const GeomShape & shape, Vector vec)
{
    BRepPrimAPI_MakePrism result(shape, (gp_Vec) vec);
    result.Build();
    if (!result.IsDone())
        throw Exception("extrude failed");
    return GeomShape(result.Shape());
}

GeomShape
revolve(const GeomShape & shape, const Axis1 & axis, double angle)
{
    BRepPrimAPI_MakeRevol result(shape, axis, angle);
    result.Build();
    if (!result.IsDone())
        throw Exception("revolve failed");
    return GeomShape(result.Shape());
}

GeomShape
rotate(const GeomShape & shape, const Axis1 & axis, double angle)
{
    gp_Trsf trsf;
    trsf.SetRotation(axis, angle);
    BRepBuilderAPI_Transform brep_trsf(shape, trsf);
    return GeomShape(brep_trsf.Shape());
}

Wire
section(const GeomShape & shape, const Plane & plane)
{
    BRepAlgoAPI_Section result(shape, plane);
    result.Build();
    if (!result.IsDone())
        throw Exception("Section operation failed");
    auto & section_edges = result.SectionEdges();
    BRepBuilderAPI_MakeWire wire;
    wire.Add(section_edges);
    wire.Build();
    if (!wire.IsDone())
        throw Exception("Wire was not created");
    return Wire(wire.Wire());
}

GeomShape
draft(const GeomShape & shape,
      const Plane & pln,
      const std::vector<GeomSurface> & faces,
      double angle)
{
    auto dir = pln.axis().direction();
    BRepOffsetAPI_DraftAngle drft(shape);
    for (auto & f : faces) {
        drft.Add(f, dir, angle, pln);
        if (!drft.AddDone())
            throw Exception("Faulty face was given");
    }
    drft.Build();
    if (drft.IsDone())
        return GeomShape(drft.Shape());
    else
        throw Exception("Draft was not constructed");
}

GeomShape
hole(const GeomShape & shape, const Axis1 & axis, double diameter)
{
    BRepFeat_MakeCylindricalHole h;
    h.Init(shape, axis);
    h.Perform(diameter / 2.);
    h.Build();
    if (h.Status() == BRepFeat_NoError)
        return GeomShape(h.Shape());
    else
        throw Exception("Hole did not generate");
}

GeomShape
hole(const GeomShape & shape, const Axis1 & axis, double diameter, double length)
{
    BRepFeat_MakeCylindricalHole h;
    h.Init(shape, axis);
    h.PerformBlind(diameter / 2., length);
    h.Build();
    if (h.Status() == BRepFeat_NoError)
        return GeomShape(h.Shape());
    else
        throw Exception("Hole did not generate");
}

GeomShape
sweep(const GeomShape & profile, const Wire & spine)
{
    BRepOffsetAPI_MakePipe mk(spine, profile);
    mk.Build();
    if (mk.IsDone())
        return GeomShape(mk.Shape());
    else
        throw Exception("Sweep was not constructed");
}

GeomShape
sew(const std::vector<GeomShape> & faces, double tol)
{
    BRepBuilderAPI_Sewing sewing_tool(tol);
    for (auto & face : faces)
        sewing_tool.Add(face);
    sewing_tool.Perform();
    return GeomShape(sewing_tool.SewedShape());
}

void
smooth(Ptr<MeshSurface> surface, int iterations)
{
    LoggingTimer timer;
    Log::info("Applying Laplace smoothing on surface {} ({} iterations)",
              surface->id(),
              iterations);

    // 1. Build adjacency map for internal vertices
    std::map<Ptr<MeshVertexAbstract>, std::set<Ptr<MeshVertexAbstract>>> neighbors;

    for (auto vtx : surface->surface_vertices())
        neighbors[vtx] = {};

    auto add_neighbors = [&](const MeshElement & elem) {
        auto vtxs = elem.vertices();
        for (auto i : make_range(vtxs.size())) {
            auto vtx = vtxs[i];
            auto it = neighbors.find(vtx);
            if (it != neighbors.end()) {
                for (auto j : make_range(vtxs.size())) {
                    if (i != j) {
                        it->second.insert(vtxs[j]);
                    }
                }
            }
        }
    };

    for (auto tri : surface->triangles())
        add_neighbors(tri);
    for (auto quad : surface->quadrangles())
        add_neighbors(quad);

    // 2. Perform Laplace iterations
    for (auto iter : make_range(iterations)) {
        (void) iter;
        std::map<Ptr<MeshVertexAbstract>, Point> new_positions;
        for (const auto & [vtx, adj] : neighbors) {
            Point avg(0, 0, 0);
            for (const auto & neighbor : adj)
                avg += neighbor->point();

            if (adj.size() > 0) {
                avg *= 1.0 / adj.size();
                new_positions[vtx] = avg;
            }
        }

        // Relocate vertices
        for (const auto & [vtx, pos] : new_positions)
            vtx->relocate(pos);
    }
}

} // namespace krado
