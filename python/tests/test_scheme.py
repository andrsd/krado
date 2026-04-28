import pytest
import krado
import os

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")

def test_set_scheme_curve():
    file_name = os.path.join(assets_dir, "box.step")
    step = krado.STEPFile(file_name)
    shapes = step.load()
    model = krado.GeomModel(shapes[0])

    curve = model.curve(1)
    curve.set_scheme("equal", intervals=50)
    # We can't really check the scheme was set without more API,
    # but we can at least check that the method doesn't throw.

def test_set_scheme_surface():
    file_name = os.path.join(assets_dir, "box.step")
    step = krado.STEPFile(file_name)
    shapes = step.load()
    model = krado.GeomModel(shapes[0])

    surface = model.surface(1)
    surface.set_scheme("triangle", max_area=0.1)

def test_set_scheme_volume():
    file_name = os.path.join(assets_dir, "box.step")
    step = krado.STEPFile(file_name)
    shapes = step.load()
    model = krado.GeomModel(shapes[0])

    volume = model.volume(1)
    volume.set_scheme("trisurf", linear_deflection=0.1, angular_deflection=0.1, is_relative=True)
