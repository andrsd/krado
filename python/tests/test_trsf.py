import math

import krado
import pytest


def test_identity():
    trsf = krado.Trsf.identity()
    pt = krado.Point(2.0, 3.0, 4.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 2.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 3.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 4.0, abs_tol=1e-12)


def test_scale_isotropic():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.identity().scale(2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 4.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 6.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 8.0, abs_tol=1e-12)


def test_scale_anisotropic():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.identity().scale(3.0, 5.0, 9.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 6.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 15.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 36.0, abs_tol=1e-12)


def test_translate():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.identity().translate(3.0, 5.0, 9.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 5.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 8.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 13.0, abs_tol=1e-12)


def test_rotate_x():
    pt = krado.Point(0.0, 2.0, 0.0)
    trsf = krado.Trsf.identity().rotate_x(math.pi / 2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 2.0, abs_tol=1e-12)


def test_rotate_y():
    pt = krado.Point(2.0, 0.0, 0.0)
    trsf = krado.Trsf.identity().rotate_y(math.pi / 2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, -2.0, abs_tol=1e-12)


def test_rotate_z():
    pt = krado.Point(2.0, 0.0, 0.0)
    trsf = krado.Trsf.identity().rotate_z(math.pi / 2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 2.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 0.0, abs_tol=1e-12)


def test_scaled_isotropic():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.scaled(2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 4.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 6.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 8.0, abs_tol=1e-12)


def test_scaled_anisotropic():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.scaled(3.0, 5.0, 9.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 6.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 15.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 36.0, abs_tol=1e-12)


def test_translated():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.translated(3.0, 5.0, 9.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 5.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 8.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 13.0, abs_tol=1e-12)


def test_rotated_x():
    pt = krado.Point(0.0, 2.0, 0.0)
    trsf = krado.Trsf.rotated_x(math.pi / 2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 2.0, abs_tol=1e-12)


def test_rotated_y():
    pt = krado.Point(2.0, 0.0, 0.0)
    trsf = krado.Trsf.rotated_y(math.pi / 2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, -2.0, abs_tol=1e-12)


def test_rotated_z():
    pt = krado.Point(2.0, 0.0, 0.0)
    trsf = krado.Trsf.rotated_z(math.pi / 2.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 2.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 0.0, abs_tol=1e-12)


def test_chained_ops_1():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.scaled(2.0) * krado.Trsf.translated(3.0, 5.0, 9.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 7.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 11.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 17.0, abs_tol=1e-12)


def test_chained_ops_2():
    pt = krado.Point(2.0, 3.0, 4.0)
    trsf = krado.Trsf.identity().scale(2.0).translate(3.0, 5.0, 9.0)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 7.0, abs_tol=1e-12)
    assert math.isclose(pt2.y, 11.0, abs_tol=1e-12)
    assert math.isclose(pt2.z, 17.0, abs_tol=1e-12)
