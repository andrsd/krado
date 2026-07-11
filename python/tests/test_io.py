import math
import os

import krado

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_import_geom_step():
    file_name = os.path.join(assets_dir, "geo", "box.step")
    shapes = krado.import_geometry(file_name)
    assert len(shapes) == 1
    assert math.isclose(shapes[0].volume(), 24.0, abs_tol=1e-8)


def test_import_geom_iges():
    file_name = os.path.join(assets_dir, "geo", "box.iges")
    shapes = krado.import_geometry(file_name)
    assert len(shapes) == 1
    assert math.isclose(shapes[0].volume(), 1.0, abs_tol=1e-8)


def test_export_geom_single(tmp_path):
    box = krado.Box(krado.Point(0, 0, 0), krado.Point(2, 3, 4))
    krado.export_geometry(box, tmp_path / "box.step")
    krado.export_geometry(box, tmp_path / "box.iges")


def test_export_geom_list(tmp_path):
    box1 = krado.Box(krado.Point(0, 0, 0), krado.Point(2, 3, 4))
    box2 = krado.Box(krado.Point(5, 0, 0), krado.Point(6, 1, 1))
    krado.export_geometry([box1, box2], tmp_path / "boxes.step")
    krado.export_geometry([box1, box2], tmp_path / "boxes.iges")
