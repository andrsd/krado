import pytest
import krado


def test_component():
    a = krado.Vector(1., 2., 3.)
    assert a.x == 1.
    assert a.y == 2.
    assert a.z == 3.


def test_add():
    a = krado.Vector(1., 2., 3.)
    b = krado.Vector(4., 5., 6.)
    c = a + b
    assert c.x == 5.
    assert c.y == 7.
    assert c.z == 9.

    d = a
    d += c
    assert d.x == 6.
    assert d.y == 9.
    assert d.z == 12.


def test_subtract():
    a = krado.Vector(1., 2., 3.)
    b = krado.Vector(6., 5., 4.)
    c = a - b
    assert c.x == -5.
    assert c.y == -3.
    assert c.z == -1.

    d = a
    d -= c
    assert d.x == 6.
    assert d.y == 5.
    assert d.z == 4.


def test_mult_scalar():
    a = krado.Vector(1, 2, 3)
    b = 2. * a
    assert b.x == 2.
    assert b.y == 4.
    assert b.z == 6.

    c = a * 3
    assert c.x == 3.
    assert c.y == 6.
    assert c.z == 9.

    d = a
    d *= 4
    assert d.x == 4.
    assert d.y == 8.
    assert d.z == 12.


def test_invert():
    a = krado.Vector(1, 2, 3)
    b = -a

    assert b.x == -1.
    assert b.y == -2.
    assert b.z == -3.


def test_norm():
    a = krado.Vector(1, 2, 3)
    assert a.norm() == 3.7416573867739413


def test_normalize():
    a = krado.Vector(2, 3, 5)
    a.normalize()
    assert a.norm() == 1.


def test_is_equal():
    a = krado.Vector(1, 0, 0)
    b = krado.Vector(1, 0, 0)
    c = krado.Vector(-1, 0, 0)
    assert a.is_equal(b)
    assert not a.is_equal(c)
