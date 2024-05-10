import pytest
import krado
import os

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_scheme_equal():
    file_name = os.path.join(assets_dir, "line.step")
    step = krado.STEPFile(file_name)
    shape = step.load()
    model = krado.GeomModel(shape)
    mesh = krado.Mesh(model)
    mesh.curve(3) \
        .set_scheme("equal") \
        .set("intervals", 4)
    mesh.mesh_curve(3)

    crv = mesh.curve(3)
    verts = crv.all_vertices()
    assert len(verts) == 5
    segs = crv.segments()
    assert len(segs) == 4
