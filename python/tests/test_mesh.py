import math
import os

import krado
import pytest

root_dir = os.path.normpath(os.path.join(__file__, "..", "..", ".."))
assets_dir = os.path.join(root_dir, "test", "assets")


def test_mesh_set_cell_set_name():
    mesh = krado.Mesh()
    mesh.set_cell_set_name(1, "test_cell_set")
    assert mesh.cell_set_name(1) == "test_cell_set"


def test_mesh_cell_set_ids():
    mesh = krado.Mesh()
    mesh.set_cell_set_name(1, "test_cell_set_1")
    mesh.set_cell_set(1, [0])  # Add a dummy cell to make the ID appear
    mesh.set_cell_set_name(2, "test_cell_set_2")
    mesh.set_cell_set(2, [1])  # Add a dummy cell to make the ID appear
    ids = mesh.cell_set_ids()
    assert isinstance(ids, list)
    assert 1 in ids
    assert 2 in ids


def disabled_test_mesh_set_cell_set():
    mesh = krado.Mesh()
    mesh.set_cell_set(1, [0, 1, 2])
    cell_set = mesh.cell_set(1)
    assert cell_set == [0, 1, 2]


def test_mesh_remove_cell_sets():
    mesh = krado.Mesh()
    mesh.set_cell_set_name(1, "test_cell_set")
    mesh.set_cell_set(1, [0, 1, 2])
    mesh.remove_cell_sets()
    assert not mesh.cell_set_ids()


def test_mesh_remap_block_ids():
    # This requires a mesh with blocks, so we'll load one or create a dummy structure
    # For simplicity, we'll test the binding logic, assuming C++ remap works.
    mesh = krado.Mesh()
    mesh.set_cell_set(10, [0, 1])
    mesh.set_cell_set(20, [2, 3])

    initial_ids = mesh.cell_set_ids()
    assert 10 in initial_ids and 20 in initial_ids

    remap = {10: 100, 20: 200}
    mesh.remap_block_ids(remap)

    remapped_ids = mesh.cell_set_ids()
    assert 100 in remapped_ids and 200 in remapped_ids
    assert 10 not in remapped_ids and 20 not in remapped_ids


def test_mesh_create_side_set(tmp_path):
    pts = [
        krado.Point(0.0, 0.0),
        krado.Point(1.0, 0.0),
        krado.Point(1.0, 1.0),
        krado.Point(0.0, 1.0),
    ]
    elems = [krado.Element(krado.ElementType.QUAD4, [0, 1, 2, 3])]

    mesh = krado.Mesh(pts, elems)
    mesh.set_up()
    bnd_edges = mesh.boundary_edges()
    mesh.create_side_set(10, [bnd_edges[0]])
    mesh.create_side_set(11, [bnd_edges[2]])
    # assert bnd_edges == [0, 1, 2]

    temp_file = tmp_path / "krado_1234.exo"
    f = krado.ExodusIIFile(temp_file)
    f.write(mesh)
    del f

    f = krado.ExodusIIFile(temp_file)
    mesh_rd = f.read()
    assert mesh_rd.side_set_ids() == [10, 11]
