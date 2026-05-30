import os

import krado
import pytest

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_set_scheme_tricircle_quad():
    file_name = os.path.join(assets_dir, "geo", "circle.step")
    step = krado.STEPFile(file_name)
    shapes = step.read()
    model = krado.GeomModel(shapes[0])

    model.curve(1).set_scheme("equal", intervals=8)

    surface = model.surface(1)
    surface.set_scheme("tricircle", symmetry_type="quadrant", radial_intervals=2)

    model.mesh_surface(1)


def test_set_scheme_tricircle_hex():
    file_name = os.path.join(assets_dir, "geo", "circle.step")
    step = krado.STEPFile(file_name)
    shapes = step.read()
    model = krado.GeomModel(shapes[0])

    model.curve(1).set_scheme("equal", intervals=18)

    surface = model.surface(1)
    surface.set_scheme("tricircle", symmetry_type="hexagonal", radial_intervals=3)

    model.mesh_surface(1)
