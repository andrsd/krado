import pytest
import krado
import os
import math


root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_area_of_a_square():
    file_name = os.path.join(assets_dir, "square-half-tri.e")
    mesh = krado.import_mesh(file_name)
    vols = krado.compute_volume(mesh)
    assert math.isclose(vols[0], 4., abs_tol=1e-10)


def test_volume_of_cube():
    file_name = os.path.join(assets_dir, "cube-tet.e")
    mesh = krado.import_mesh(file_name)
    vols = krado.compute_volume(mesh)
    assert math.isclose(vols[0], 1., abs_tol=1e-10)
