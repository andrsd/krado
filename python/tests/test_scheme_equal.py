import pytest
import krado
import os

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_scheme_equal():
    file_name = os.path.join(assets_dir, "line.step")
    step = krado.STEPFile(file_name)
    shapes = step.load()
    model = krado.GeomModel(shapes[0])
    model.curve(1).set_scheme("equal").set(intervals=4)

    sch = model.curve(1).scheme()
    print(sch.name())
    params = sch.get()
    print(params)
    assert params["intervals"] == 4

    model.mesh_curve(1)

    crv = model.curve(1)
    verts = crv.all_vertices()
    assert len(verts) == 5
    segs = crv.segments()
    assert len(segs) == 4
