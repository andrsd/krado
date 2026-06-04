import math
import os

import krado
import pytest

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_area_of_a_square():
    file_name = os.path.join(assets_dir, "mesh", "cube-tet.e")
    mesh = krado.import_mesh(file_name)
    mesh.set_up()

    cyl = krado.Cylinder(
        krado.Axis2(krado.Point(1.0, 0.0, 0.0), krado.Vector(0.0, 0.0, 1)), 0.5, 2
    )
    cyl_clsf = krado.SolidClassifier(cyl)

    inside_facets = []
    facet_ids = mesh.boundary_faces()
    for id in facet_ids:
        centroid = mesh.compute_centroid(id)
        if cyl_clsf.inside(centroid):
            inside_facets.append(id)

    assert inside_facets == [23, 30]
