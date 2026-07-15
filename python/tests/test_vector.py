import krado
import pytest


def test_component():
    a = krado.Vector(1.0, 2.0, 3.0)
    assert isinstance(a, krado.Vector)
    assert a.x == 1.0
    assert a.y == 2.0
    assert a.z == 3.0


def test_add():
    a = krado.Vector(1.0, 2.0, 3.0)
    b = krado.Vector(4.0, 5.0, 6.0)
    c = a + b
    assert isinstance(c, krado.Vector)
    assert c.x == 5.0
    assert c.y == 7.0
    assert c.z == 9.0

    d = a
    d += c
    assert isinstance(d, krado.Vector)
    assert d.x == 6.0
    assert d.y == 9.0
    assert d.z == 12.0


def test_subtract():
    a = krado.Vector(1.0, 2.0, 3.0)
    b = krado.Vector(6.0, 5.0, 4.0)
    c = a - b
    assert isinstance(c, krado.Vector)
    assert c.x == -5.0
    assert c.y == -3.0
    assert c.z == -1.0

    d = a
    d -= c
    assert isinstance(d, krado.Vector)
    assert d.x == 6.0
    assert d.y == 5.0
    assert d.z == 4.0


def test_mult_scalar():
    a = krado.Vector(1, 2, 3)
    b = 2.0 * a
    assert isinstance(b, krado.Vector)
    assert b.x == 2.0
    assert b.y == 4.0
    assert b.z == 6.0

    c = a * 3
    assert isinstance(c, krado.Vector)
    assert c.x == 3.0
    assert c.y == 6.0
    assert c.z == 9.0

    d = a
    d *= 4
    assert isinstance(d, krado.Vector)
    assert d.x == 4.0
    assert d.y == 8.0
    assert d.z == 12.0


def test_invert():
    a = krado.Vector(1, 2, 3)
    b = -a
    assert isinstance(b, krado.Vector)
    assert b.x == -1.0
    assert b.y == -2.0
    assert b.z == -3.0


def test_norm():
    a = krado.Vector(1, 2, 3)
    assert a.norm() == 3.7416573867739413


def test_normalize():
    a = krado.Vector(2, 3, 5)
    a.normalize()
    assert a.norm() == 1.0


def test_is_equal():
    a = krado.Vector(1, 0, 0)
    b = krado.Vector(1, 0, 0)
    c = krado.Vector(-1, 0, 0)
    assert a.is_equal(b)
    assert not a.is_equal(c)
