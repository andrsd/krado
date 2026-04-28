import os

import krado
import pytest

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_geom_model_indexing():
    file_name = os.path.join(assets_dir, "box.step")
    step = krado.STEPFile(file_name)
    shapes = step.load()
    model = krado.GeomModel(shapes[0])

    # Test that we can access elements using integers (ShapeID wrapping)
    # The box.step should have some vertices, curves, surfaces, volumes

    # Vertices
    verts = model.vertices()
    assert len(verts) > 0
    v1 = model.vertex(1)
    assert isinstance(v1, krado.MeshVertex)

    # Curves
    curves = model.curves()
    assert len(curves) > 0
    c1 = model.curve(1)
    assert isinstance(c1, krado.MeshCurve)

    # Test method call on Ptr-held object
    c1.set_marker(104)
    assert c1.marker() == 104

    # Surfaces
    surfs = model.surfaces()
    assert len(surfs) > 0
    s1 = model.surface(1)
    assert isinstance(s1, krado.MeshSurface)

    # Volumes
    vols = model.volumes()
    assert len(vols) > 0
    vol1 = model.volume(1)
    assert isinstance(vol1, krado.MeshVolume)


def test_shape_id_class():
    sid = krado.ShapeID(5)
    assert sid.value() == 5


def test_build_from_geom_surface():
    pt1 = krado.Point(0, 0, 0)
    pt2 = krado.Point(1, 0, 0)
    pt3 = krado.Point(1, 1, 0)
    poly = krado.Polygon([pt1, pt2, pt3])

    surf = krado.GeomSurface(poly)
    model = krado.GeomModel(surf)

    surfs = model.surfaces()
    assert len(surfs) > 0
