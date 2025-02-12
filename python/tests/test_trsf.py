import pytest
import krado
import math


def test_identity():
    trsf = krado.Trsf.identity()
    pt = krado.Point(2., 3., 4.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 2., abs_tol=1e-12)
    assert math.isclose(pt2.y, 3., abs_tol=1e-12)
    assert math.isclose(pt2.z, 4., abs_tol=1e-12)


def test_scale_isotropic():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.identity().scale(2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 4., abs_tol=1e-12)
    assert math.isclose(pt2.y, 6., abs_tol=1e-12)
    assert math.isclose(pt2.z, 8., abs_tol=1e-12)


def test_scale_anisotropic():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.identity().scale(3., 5., 9.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 6., abs_tol=1e-12)
    assert math.isclose(pt2.y, 15., abs_tol=1e-12)
    assert math.isclose(pt2.z, 36., abs_tol=1e-12)


def test_translate():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.identity().translate(3., 5., 9.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 5., abs_tol=1e-12)
    assert math.isclose(pt2.y, 8., abs_tol=1e-12)
    assert math.isclose(pt2.z, 13., abs_tol=1e-12)


def test_rotate_x():
    pt = krado.Point(0., 2., 0.)
    trsf = krado.Trsf.identity().rotate_x(math.pi/2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0., abs_tol=1e-12)
    assert math.isclose(pt2.y, 0., abs_tol=1e-12)
    assert math.isclose(pt2.z, 2., abs_tol=1e-12)


def test_rotate_y():
    pt = krado.Point(2., 0., 0.)
    trsf = krado.Trsf.identity().rotate_y(math.pi/2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0., abs_tol=1e-12)
    assert math.isclose(pt2.y, 0., abs_tol=1e-12)
    assert math.isclose(pt2.z, -2., abs_tol=1e-12)


def test_rotate_z():
    pt = krado.Point(2., 0., 0.)
    trsf = krado.Trsf.identity().rotate_z(math.pi/2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0., abs_tol=1e-12)
    assert math.isclose(pt2.y, 2., abs_tol=1e-12)
    assert math.isclose(pt2.z, 0., abs_tol=1e-12)


def test_scaled_isotropic():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.scaled(2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 4., abs_tol=1e-12)
    assert math.isclose(pt2.y, 6., abs_tol=1e-12)
    assert math.isclose(pt2.z, 8., abs_tol=1e-12)


def test_scaled_anisotropic():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.scaled(3., 5., 9.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 6., abs_tol=1e-12)
    assert math.isclose(pt2.y, 15., abs_tol=1e-12)
    assert math.isclose(pt2.z, 36., abs_tol=1e-12)


def test_translated():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.translated(3., 5., 9.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 5., abs_tol=1e-12)
    assert math.isclose(pt2.y, 8., abs_tol=1e-12)
    assert math.isclose(pt2.z, 13., abs_tol=1e-12)


def test_rotated_x():
    pt = krado.Point(0., 2., 0.)
    trsf = krado.Trsf.rotated_x(math.pi/2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0., abs_tol=1e-12)
    assert math.isclose(pt2.y, 0., abs_tol=1e-12)
    assert math.isclose(pt2.z, 2., abs_tol=1e-12)


def test_rotated_y():
    pt = krado.Point(2., 0., 0.)
    trsf = krado.Trsf.rotated_y(math.pi/2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0., abs_tol=1e-12)
    assert math.isclose(pt2.y, 0., abs_tol=1e-12)
    assert math.isclose(pt2.z, -2., abs_tol=1e-12)


def test_rotated_z():
    pt = krado.Point(2., 0., 0.)
    trsf = krado.Trsf.rotated_z(math.pi/2.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 0., abs_tol=1e-12)
    assert math.isclose(pt2.y, 2., abs_tol=1e-12)
    assert math.isclose(pt2.z, 0., abs_tol=1e-12)


def test_chained_ops_1():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.scaled(2.) * krado.Trsf.translated(3., 5., 9.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 7., abs_tol=1e-12)
    assert math.isclose(pt2.y, 11., abs_tol=1e-12)
    assert math.isclose(pt2.z, 17., abs_tol=1e-12)


def test_chained_ops_2():
    pt = krado.Point(2., 3., 4.)
    trsf = krado.Trsf.identity().scale(2.).translate(3., 5., 9.)
    pt2 = trsf * pt
    assert math.isclose(pt2.x, 7., abs_tol=1e-12)
    assert math.isclose(pt2.y, 11., abs_tol=1e-12)
    assert math.isclose(pt2.z, 17., abs_tol=1e-12)
