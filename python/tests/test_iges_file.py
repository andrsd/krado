import os

import krado
import pytest

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_load():
    file_name = os.path.join(assets_dir, "geo", "box.iges")
    step = krado.IGESFile(file_name)
    step.read()
