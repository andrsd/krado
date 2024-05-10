import pytest
import krado
import os

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")

def test_load():
    file_name = os.path.join(assets_dir, "box.step")
    step = krado.STEPFile(file_name)
    step.load()
